#include <avr/io.h>
#include <util/delay.h>

#include "error.h"
#include "flash.h"
#include "spi.h"
#include "uart.h"

// w25q128 instructions
#define FLASH_PAGE_PROGRAM    0x02
#define FLASH_READ_DATA       0x03
#define FLASH_READ_STATUS_REG 0x05
#define FLASH_WRITE_ENABLE    0x06
// 0x60 would also work
#define FLASH_CHIP_ERASE 0xc7

static uint32_t          next_free_block_addr = 0;
static GenericFlashBlock block_buf;

uint8_t flash_is_full()
{
    return next_free_block_addr == FLASH_SIZE;
}

void flash_write_enable()
{
    spi_start();
    spi_transceive_char(FLASH_WRITE_ENABLE);
    spi_end();
}

int flash_is_busy()
{
    spi_start();
    spi_transceive_char(FLASH_READ_STATUS_REG);

    char reg1 = spi_receive_char();
    spi_end();

    return reg1 & (1 << 0);
}

void flash_await_write_completion()
{
    while(flash_is_busy())
        ;
}

void send_address(uint32_t address)
{
    // MSB address
    spi_transceive_char((address >> 0x10) & 0xff);
    spi_transceive_char((address >> 0x8) & 0xff);
    spi_transceive_char((address >> 0x0) & 0xff);
}

int flash_is_data_valid(uint32_t address, void* data, uint8_t nbytes)
{
    MD_ASSERT(data, MOLD_ERROR_INVALID_PARAMS_FLASH_IS_DATA_VALID_DATA_IS_NULL);

    spi_start();
    spi_transceive_char(FLASH_READ_DATA);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        if(((char*)data)[i] != spi_receive_char()) {
            spi_end();
            return 0;
        }
    }

    spi_end();
    return 1;
}

// write nbytes bytes from flash chip at address address to buf
// The flash chip may not be in power down mode
void flash_read_data(uint32_t address, void* buf, uint8_t nbytes)
{
    MD_ASSERT(buf, MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_BUF_IS_NULL);
    MD_ASSERT(address + nbytes <= FLASH_SIZE, MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_ADDRESS_OR_NBYTES_IS_TOO_HIGH);

    spi_start();
    spi_transceive_char(FLASH_READ_DATA);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        ((char*)buf)[i] = spi_receive_char();
    }

    spi_end();
}

// Write nbytes bytes from data to flash chip starting at address address.
// The area written to must be erased beforehand.
// The flash chip may not be in power down mode
// more than 256 bytes (one page) can't be written with one write command
//
// excerpt from data sheet p. 37:
//
// "If an entire 256 byte page is to be programmed, the last address byte (the 8 least significant address bits)
// should be set to 0. If the last address byte is not zero, and the number of clocks exceeds the remaining
// page length, the addressing will wrap to the beginning of the page. In some cases, less than 256 bytes (a
// partial page) can be programmed without having any effect on other bytes within the same page. One
// condition to perform a partial page program is that the number of clocks cannot exceed the remaining
// page length. If more than 256 bytes are sent to the device the addressing will wrap to the beginning of the
// page and overwrite previously sent data."
void flash_write_data(uint32_t address, void* data, uint8_t nbytes)
{
    MD_ASSERT(data, MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_DATA_DATA_IS_NULL);

    flash_write_enable();
    spi_start();

    spi_transceive_char(FLASH_PAGE_PROGRAM);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        spi_transceive_char(((char*)data)[i]);
    }

    spi_end();
    flash_await_write_completion();

    MD_ASSERT(flash_is_data_valid(address, data, nbytes), MOLD_ERROR_FLASH_WRITE_DATA_VALIDATION_FAILED);
}

void flash_read_block(uint32_t address, GenericFlashBlock* block)
{
    MD_ASSERT(block, MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_BLOCK_IS_NULL);
    // address needs to start at a block
    MD_ASSERT((address & FLASH_BLOCK_ADDR_MASK) == 0, MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK);

    flash_read_data(address, block, sizeof(GenericFlashBlock));
}

void flash_write_block(uint32_t address, GenericFlashBlock* block)
{
    MD_ASSERT(block, MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_BLOCK_ADDRESS_BLOCK_IS_NULL);
    // address needs to start at a block
    MD_ASSERT((address & FLASH_BLOCK_ADDR_MASK) == 0, MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK);

    flash_write_data(address, block, sizeof(GenericFlashBlock));
}

uint8_t is_block_free(uint32_t address)
{
    // address needs to start at a block
    MD_ASSERT((address & FLASH_BLOCK_ADDR_MASK) == 0, MOLD_ERROR_INVALID_PARAMS_IS_BLOCK_FREE_ADDRESS_DOESNT_START_AT_BLOCK);

    // this could be optimized by only reading the flag
    flash_read_block(address, &block_buf);

    return flash_is_block_free(block_buf.flags);
}

void flash_check_correct_free_block_addr()
{
    MD_ASSERT(flash_is_full() || is_block_free(next_free_block_addr), MOLD_ERROR_FLASH_CHECK_CORRECT_FREE_BLOCK_ADDR_NEXT_FREE_BLOCK_NOT_FREE);
    MD_ASSERT(next_free_block_addr == 0 || !is_block_free(next_free_block_addr - FLASH_BLOCK_SIZE), MOLD_ERROR_FLASH_CHECK_CORRECT_FREE_BLOCK_ADDR_BEFORE_NEXT_FREE_BLOCK_FREE);
}

// Perform a binary search to find the first free block.
void flash_find_next_free_block()
{
    // adapted from: https://www.geeksforgeeks.org/implementing-upper_bound-and-lower_bound-in-c/
    uint32_t mid;
    // the search window is [low, high)
    uint32_t low  = 0;
    uint32_t high = FLASH_BLOCKS_NUM;

    // until low is higher than high
    while(low < high) {
        mid = low + (high - low) / 2;

        if(is_block_free(mid * FLASH_BLOCK_SIZE))
            // use left subarray
            high = mid;
        else
            // use right subarray
            low = mid + 1;
    }

    // when even the last block is not free
    if(low < FLASH_BLOCKS_NUM && !is_block_free(low * FLASH_BLOCK_SIZE))
        ++low;

    next_free_block_addr = low * FLASH_BLOCK_SIZE;
    flash_check_correct_free_block_addr();
    flash_print_usage();
}

void flash_write_next_block(GenericFlashBlock* block)
{
    MD_ASSERT(block, MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_NEXT_BLOCK_BLOCK_IS_NULL);
    uart_println("attempting to write block:");
    uart_print_flash_block(block);
    uart_println("");

    MD_ASSERT(!flash_is_full(), MOLD_ERROR_FLASH_WRITE_NEXT_BLOCK_FLASH_IS_FULL);
    flash_write_block(next_free_block_addr, block);

    // Check next_free_block_addr is found correctly.
    // The binary search is so fast we can effort this check.
    uint32_t expected_next_block = next_free_block_addr + FLASH_BLOCK_SIZE;
    flash_find_next_free_block();
    MD_ASSERT(next_free_block_addr == expected_next_block, MOLD_ERROR_FLASH_WRITE_NEXT_BLOCK_FIND_NEXT_FREE_BLOCK_FAILED);
}

void flash_print_usage()
{
    if(flash_is_full())
        uart_println("Warning: flash is full");
    else {
        uart_print("Next free block is at: ");
        uart_print_uint32_t_hex(next_free_block_addr);
        uart_print(" of ");
        uart_print_uint32_t_hex(FLASH_SIZE);
        uart_println("");
    }
}

void flash_chip_erase()
{
    uart_print("Erasing chip: ...");
    flash_write_enable();
    spi_start();

    spi_transceive_char(FLASH_CHIP_ERASE);

    spi_end();
    flash_await_write_completion();

    uart_println("done");

    // check next_free_block_addr is found correctly
    flash_find_next_free_block();
    MD_ASSERT(next_free_block_addr == 0, MOLD_ERROR_FLASH_FLASH_CHIP_ERASE_FIND_NEXT_FREE_BLOCK_FAILED);
}

void flash_init()
{
    if(sizeof(GenericFlashBlock) != FLASH_BLOCK_SIZE ||
       sizeof(FlashSensorData) != FLASH_BLOCK_SIZE ||
       sizeof(FlashTimestamp) != FLASH_BLOCK_SIZE)
        return;

    spi_controller_init();
    flash_find_next_free_block();
}

void flash_print_all_blocks()
{
    uart_println("[");
    for(uint32_t addr = 0; addr < next_free_block_addr; addr += FLASH_BLOCK_SIZE) {
        flash_read_block(addr, &block_buf);
        uart_print_flash_block(&block_buf);

        // is this the last block?
        if(addr != next_free_block_addr)
            uart_println(",");
    }
    uart_println("]");
}
