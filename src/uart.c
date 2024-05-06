#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "uart.h"

static char cmd_buf[MAX_CMD_LENGTH];

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
    // max number is 255
    char str[4];
    utoa(val, str, 10);
    uart_print(str);
}
void uart_print_uint8_t_hex_raw(uint8_t val)
{
    // max number is ff
    char str[3];
    utoa(val, str, 0x10);
    uart_print(str);
}
void uart_print_uint8_t_hex(uint16_t val)
{
    uart_print("0x");
    uart_print_uint8_t_hex_raw((val >> 0x00) & 0xff);
}
void uart_print_uint16_t_hex(uint16_t val)
{
    uart_print("0x");
    uart_print_uint8_t_hex_raw((val >> 0x08) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x00) & 0xff);
}
void uart_print_uint32_t_hex(uint32_t val)
{
    uart_print("0x");
    uart_print_uint8_t_hex_raw((val >> 0x18) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x10) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x08) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x00) & 0xff);
}
void uart_print_uint64_t_hex(uint64_t val)
{
    uart_print("0x");
    uart_print_uint8_t_hex_raw((val >> 0x38) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x30) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x28) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x20) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x18) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x10) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x08) & 0xff);
    uart_print_uint8_t_hex_raw((val >> 0x00) & 0xff);
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
