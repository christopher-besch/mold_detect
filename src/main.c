#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "flash.h"
#include "uart.h"

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

int main(void)
{
    // init
    flash_init();
    led_init();
    uart_init();

    char    data[] = "How are you doing, my little flashy flash chip? You're really flashy, you know that. And you're doing a great job, keep up the good work!";
    uint8_t len    = strlen(data) + 1;
    // if(flash_sector_erase(0x0)) {
    //     uart_println("failed to erase");
    //     while(1)
    //         ;
    // }
    // uart_println(flash_write_data(0x69, data, len) ? "Write failed" : "Write succeeded");

    while(1) {
        set_led(1);
        char buf[len];
        flash_read_data(0x69, buf, len);
        uart_println(buf);
        set_led(0);
        _delay_ms(500);
    }
}
