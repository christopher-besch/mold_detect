#include <avr/delay.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void set_led(uint8_t state)
{
    if(state) {
        // set PORTD2 to high
        PORTD |= 1 << PORT2;
    }
    else {
        // set PORTD2 to low
        PORTD &= ~(1 << PORT2);
    }
}

void led_init()
{
    // set DDD2 to output
    DDRD |= 1 << DD2;
    set_led(0);
}

void uart_trans(unsigned char data)
{
    // wait for empty transmit buffer
    while(!(UCSR0A & (1 << UDRE0)))
        ;

    // send data
    UDR0 = data;
}

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

int main(void)
{
    led_init();
    uart_init();
    while(1) {
        set_led(1);
        uart_trans('h');
        uart_trans('e');
        uart_trans('l');
        uart_trans('l');
        uart_trans('o');
        uart_trans(' ');
        uart_trans('w');
        uart_trans('o');
        uart_trans('r');
        uart_trans('l');
        uart_trans('d');
        uart_trans('\r');
        uart_trans('\n');
        set_led(0);
        _delay_ms(500);
    }
}
