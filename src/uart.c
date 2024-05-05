#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "uart.h"

void uart_init()
{
    // do a make clean then make all to use an updated F_CPU value
#define BAUD   9600
#define MYUBRR (F_CPU / 16 / BAUD - 1)
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;

    // calculated by <util/setbaud.h>
    // UBRR0H = UBRRH_VALUE;
    // UBRR0L = UBRRL_VALUE;
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

void uart_print_uint8_t(uint8_t val)
{
    char str[4];
    utoa(val, str, 10);
    uart_print(str);
}
