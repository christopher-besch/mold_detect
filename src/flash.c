#include <avr/io.h>
#include <util/delay.h>

#include "error.h"
#include "flash.h"
#include "interrupts.h"
#include "spi.h"
#include "uart.h"

// w25q128 instructions
#define FLASH_PAGE_PROGRAM    0x02
#define FLASH_READ_DATA       0x03
#define FLASH_READ_STATUS_REG 0x05
#define FLASH_WRITE_ENABLE    0x06
#define FLASH_SECTOR_ERASE    0x20

static uint32_t          next_free_block_addr = 0;
static GenericFlashBlock free_check_flash_block;

void flash_init()
{
    if(sizeof(GenericFlashBlock) != FLASH_BLOCK_SIZE ||
       sizeof(FlashSensorData) != FLASH_BLOCK_SIZE ||
       sizeof(FlashTimestamp) != FLASH_BLOCK_SIZE)
        return;

    spi_controller_init();
    flash_find_next_free_block();
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

uint8_t flash_is_sector_erased(uint32_t address)
{
    // address needs to be at the start of a sector
    if(address & 0xfff) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_IS_SECTOR_ERASED_NOT_AT_START_OF_SECTOR);
        reset();
    }

    spi_start();
    spi_transceive_char(FLASH_READ_DATA);
    send_address(address);

    for(uint16_t i = 0; i < 0xfff; ++i) {
        if(spi_receive_char() != (char)0xff) {
            spi_end();
            return 0;
        }
    }

    spi_end();
    return 1;
}

int flash_is_data_valid(uint32_t address, void* data, uint8_t nbytes)
{
    if(!data) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_IS_DATA_VALID_DATA_IS_NULL);
        reset();
    }

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

void flash_sector_erase(uint32_t address)
{
    if(address & 0xfff) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_SECTOR_ERASE_NOT_AT_START_OF_SECTOR);
        reset();
    }

    flash_write_enable();
    spi_start();

    spi_transceive_char(FLASH_SECTOR_ERASE);
    send_address(address);

    spi_end();
    flash_await_write_completion();

    if(!flash_is_sector_erased(address)) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_SECTOR_ERASE_ERASE_FAILED);
        reset();
    }
}

void flash_read_data(uint32_t address, void* buf, uint8_t nbytes)
{
    if(!buf) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_BUF_IS_NULL);
        reset();
    }
    if(address + nbytes >= FLASH_SIZE) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_ADDRESS_OR_NBYTES_IS_TOO_HIGH);
        reset();
    }

    spi_start();
    spi_transceive_char(FLASH_READ_DATA);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        ((char*)buf)[i] = spi_receive_char();
    }

    spi_end();
}

void flash_write_data(uint32_t address, void* data, uint8_t nbytes)
{
    if(!data) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_DATA_DATA_IS_NULL);
        reset();
    }

    flash_write_enable();
    spi_start();

    spi_transceive_char(FLASH_PAGE_PROGRAM);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        spi_transceive_char(((char*)data)[i]);
    }

    spi_end();
    flash_await_write_completion();

    if(!flash_is_data_valid(address, data, nbytes)) {
        raise_error(MOLD_ERROR_FLASH_WRITE_DATA_VALIDATION_FAILED);
        reset();
    }
}

void flash_read_block(uint32_t address, GenericFlashBlock* block)
{
    if(!block) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_BLOCK_IS_NULL);
        reset();
    }
    // address needs to start at a block
    if(address & FLASH_BLOCK_ADDR_MASK) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK);
        reset();
    }

    flash_read_data(address, block, sizeof(GenericFlashBlock));
}

uint8_t is_block_free(uint32_t address)
{
    uart_print("is_block_free: ");
    uart_print_uint32_t_hex(address);
    uart_println("");
    // address needs to start at a block
    if(address & FLASH_BLOCK_ADDR_MASK) {
        raise_error(MOLD_ERROR_INVALID_PARAMS_IS_BLOCK_FREE_ADDRESS_DOESNT_START_AT_BLOCK);
        reset();
    }

    // this could be optimized by only reading the flag
    flash_read_block(address, &free_check_flash_block);

    return free_check_flash_block.flags & FLASH_BLOCK_FLAG_FREE;
}

void flash_find_next_free_block()
{
    // adapted from: https://www.geeksforgeeks.org/implementing-upper_bound-and-lower_bound-in-c/
    uint32_t mid;
    // the search window is [low, high)
    uint32_t low = 0;
    uart_print_uint32_t_hex(0x1000000);
    uart_println("");
    uart_print_uint32_t_hex(FLASH_SIZE);
    uart_println("");
    uint32_t high = FLASH_SIZE / FLASH_BLOCK_SIZE;

    // until low is higher than high
    while(low < high) {
        mid = low + (high - low) / 2;
        uart_print("low: ");
        uart_print_uint32_t_hex(low);
        uart_print(" mid: ");
        uart_print_uint32_t_hex(mid);
        uart_print(" high: ");
        uart_print_uint32_t_hex(high);
        uart_println("");

        if(is_block_free(mid * FLASH_BLOCK_SIZE))
            // use left subarray
            high = mid;
        else
            // use right subarray
            low = mid + FLASH_BLOCK_SIZE;
    }

    // when even the last block is not free
    if(low < FLASH_SIZE && !is_block_free(low * FLASH_BLOCK_SIZE))
        low += FLASH_BLOCK_SIZE;

    next_free_block_addr = low * FLASH_BLOCK_SIZE;
    if(next_free_block_addr == FLASH_SIZE)
        uart_println("Warning: flash is full");
}
