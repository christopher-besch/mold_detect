#include <avr/io.h>
#include <util/delay.h>

#include "flash.h"
#include "spi.h"
#include "uart.h"

void flash_init()
{
    spi_controller_init();
}

void flash_write_enable()
{
    spi_start();
    spi_transceive_char(0x06);
    spi_end();
}

int flash_is_busy()
{
    spi_start();
    spi_transceive_char(0x05);

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

int flash_is_sector_erased(uint32_t address)
{
    if(address & 0xfff)
        return 0;

    spi_start();
    spi_transceive_char(0x03);
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
    if(!data)
        return 0;

    spi_start();
    spi_transceive_char(0x03);
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

int flash_sector_erase(uint32_t address)
{
    if(address & 0xfff)
        return -1;

    flash_write_enable();
    spi_start();

    spi_transceive_char(0x20);
    send_address(address);

    spi_end();
    flash_await_write_completion();

    return flash_is_sector_erased(address) ? 0 : -1;
}

int flash_read_data(uint32_t address, void* buf, uint8_t nbytes)
{
    if(!buf)
        return -1;

    spi_start();
    spi_transceive_char(0x03);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        ((char*)buf)[i] = spi_receive_char();
    }

    spi_end();
    return 0;
}

int flash_write_data(uint32_t address, void* data, uint8_t nbytes)
{
    if(!data)
        return -1;

    flash_write_enable();
    spi_start();

    spi_transceive_char(0x02);
    send_address(address);

    for(uint8_t i = 0; i < nbytes; ++i) {
        spi_transceive_char(((char*)data)[i]);
    }

    spi_end();
    flash_await_write_completion();

    return flash_is_data_valid(address, data, nbytes) ? 0 : -1;
}
