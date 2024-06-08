#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "uart.h"

static char line_buf[MAX_CMD_LENGTH];
void        uart_init()
{
    // do a make clean then make all to use an updated F_CPU or BAUD value
#define MYUBRR (F_CPU / 8 / BAUD - 1)
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;

    // enable double speed uart -> cuts divider above from 16 into 8
    UCSR0A = (1 << U2X0);
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
    memset(line_buf, 0, MAX_CMD_LENGTH);
    for(uint8_t i = 0; i < MAX_CMD_LENGTH - 1; ++i) {
        char c = uart_rec();
        // exclude \r and \n in the line
        if(c == '\r')
            break;
        if(c == '\n')
            continue;
        uart_trans(c);
        line_buf[i] = c;
    }
    uart_println("");
    return line_buf;
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
