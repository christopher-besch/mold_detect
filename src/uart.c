#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "error.h"
#include "flash_blocks.h"
#include "interrupts.h"
#include "uart.h"

static char cmd_buf[MAX_CMD_LENGTH];
static char utoa_buf[5];

void uart_init()
{
    // do a make clean then make all to use an updated F_CPU value
#define BAUD   9600
#define MYUBRR (F_CPU / 16 / BAUD - 1)
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;

    // enable receiver and transmitter
    // leave UCSZ02 to 0 for 8 data bits
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // set frame format: async, no parity, 1 stop bit, 8 data bits
    UCSR0C = (1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_trans(unsigned char data)
{
    // wait for empty transmit buffer
    while(!(UCSR0A & (1 << UDRE0)))
        ;

    // send data
    UDR0 = data;
}

unsigned char uart_rec()
{
    // wait for data to be received
    while(!(UCSR0A & (1 << RXC0)))
        ;
    // return received byte
    return UDR0;
}

char* uart_rec_line()
{
    // this also sets the null terminator
    memset(cmd_buf, 0, MAX_CMD_LENGTH);
    for(uint8_t i = 0; i < MAX_CMD_LENGTH - 1; ++i) {
        char c = uart_rec();
        // exclude \r and \n in the line
        if(c == '\r')
            break;
        if(c == '\n')
            continue;
        uart_trans(c);
        cmd_buf[i] = c;
    }
    uart_println("");
    return cmd_buf;
}

void uart_print(const char* str)
{
    while(*str) {
        uart_trans(*str);
        ++str;
    }
}
void uart_println(const char* str)
{
    uart_print(str);
    uart_print("\r\n");
}

void uart_print_uint8_t_dec(uint8_t val)
{
    utoa(val, utoa_buf, 10);
    uart_print(utoa_buf);
}

void uart_print_hex_digit(uint8_t val)
{
    if(val < 0xa)
        uart_trans('0' - 0x0 + val);
    else if(val < 0x10)
        uart_trans('a' - 0xa + val);
    else {
        raise_error(MOLD_ERROR_INVALID_PARAMS_UART_PRINT_HEX_DIGIT_MORE_THAN_ONE_DIGIT);
        reset();
    }
}
void uart_print_uint8_t_hex(uint16_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_uint16_t_hex(uint16_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x0c) & 0x0f);
    uart_print_hex_digit((val >> 0x08) & 0x0f);
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_uint32_t_hex(uint32_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x1c) & 0x0f);
    uart_print_hex_digit((val >> 0x18) & 0x0f);
    uart_print_hex_digit((val >> 0x14) & 0x0f);
    uart_print_hex_digit((val >> 0x10) & 0x0f);
    uart_print_hex_digit((val >> 0x0c) & 0x0f);
    uart_print_hex_digit((val >> 0x08) & 0x0f);
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_uint64_t_hex(uint64_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x3c) & 0x0f);
    uart_print_hex_digit((val >> 0x38) & 0x0f);
    uart_print_hex_digit((val >> 0x34) & 0x0f);
    uart_print_hex_digit((val >> 0x30) & 0x0f);
    uart_print_hex_digit((val >> 0x2c) & 0x0f);
    uart_print_hex_digit((val >> 0x28) & 0x0f);
    uart_print_hex_digit((val >> 0x24) & 0x0f);
    uart_print_hex_digit((val >> 0x20) & 0x0f);
    uart_print_hex_digit((val >> 0x1c) & 0x0f);
    uart_print_hex_digit((val >> 0x18) & 0x0f);
    uart_print_hex_digit((val >> 0x14) & 0x0f);
    uart_print_hex_digit((val >> 0x10) & 0x0f);
    uart_print_hex_digit((val >> 0x0c) & 0x0f);
    uart_print_hex_digit((val >> 0x08) & 0x0f);
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_bool(uint8_t b)
{
    uart_print(b ? "true" : "false");
}

void flash_print_sensor_data_block(FlashSensorData* block)
{
    if(!block) {
        raise_error(MOLD_ERROR_FLASH_PRINT_SENSOR_DATA_BLOCK_NULL);
        reset();
    }
    uart_print("\"raw\":\"");
    uart_print_uint64_t_hex(*(uint64_t*)block);
    uart_print("\",\"type\":\"sensor_data\"");
    uart_print(",\"is_free\":");
    uart_print_bool(flash_is_block_free(block->flags));
    uart_print(",\"is_atmos_bad\":");
    uart_print_bool(flash_is_block_atmos_bad(block->flags));
    uart_print(",\"is_err_set\":");
    uart_print_bool(flash_is_block_err_set(block->flags));
    uart_print(",\"temp\":\"");
    uart_print_uint16_t_hex(block->temperature);
    uart_print("\",\"hum\":\"");
    uart_print_uint16_t_hex(block->humidity);
    uart_print("\",\"temp_crc\":\"");
    uart_print_uint16_t_hex(block->temperature_crc);
    uart_print("\",\"hum_crc\":\"");
    uart_print("\"");
    uart_print_uint16_t_hex(block->humidity_crc);
}
void flash_print_timestamp_block(FlashTimestamp* block)
{
    if(!block) {
        raise_error(MOLD_ERROR_FLASH_PRINT_TIMESTAMP_BLOCK_NULL);
        reset();
    }
    uart_print("\"raw\":\"");
    uart_print_uint64_t_hex(*(uint64_t*)block);
    uart_print("\",\"type\":\"timestamp\"");
    uart_print(",\"is_free\":");
    uart_print_bool(flash_is_block_free(block->flags));
    uart_print(",\"is_atmos_bad\":");
    uart_print_bool(flash_is_block_atmos_bad(block->flags));
    uart_print(",\"is_err_set\":");
    uart_print_bool(flash_is_block_err_set(block->flags));
    uart_print(",\"timestamp\":\"");
    uart_print_uint64_t_hex(block->unix_second_timestamp);
    uart_print("\"");
}
void uart_print_flash_block(GenericFlashBlock* block)
{
    if(!block) {
        raise_error(MOLD_ERROR_UART_PRINT_FLASH_BLOCK_NULL);
        reset();
    }

    uart_print("{");
    switch(flash_get_block_type(block->flags)) {
    case SENSOR_DATA_BLOCK:
        flash_print_sensor_data_block((FlashSensorData*)block);
        break;
    case TIMESTAMP_BLOCK:
        flash_print_timestamp_block((FlashTimestamp*)block);
        break;
    default:
        raise_error(MOLD_ERROR_UART_PRINT_FLASH_BLOCK_INVALID_TYPE);
        reset();
    }
    uart_print("}");
}
