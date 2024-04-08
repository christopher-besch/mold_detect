#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "uart.h"

void uart_init()
{
#define FOSC   2000000 // Clock Speed
#define BAUD   9600
#define MYUBRR (FOSC / 16 / BAUD - 1)
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
