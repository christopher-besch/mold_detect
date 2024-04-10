#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "flash.h"
#include "ic2.h"
#include "uart.h"

void set_led(uint8_t state)
{
    if(state) {
        // set PD2 to high
        PORTD |= 1 << PORT2;
    }
    else {
        // set PD2 to low
        PORTD &= ~(1 << PORT2);
    }
}

void led_init()
{
    // set DDD2 to output
    DDRD |= 1 << DD2;
    set_led(0);
}

void start_sleep()
{
    // set done pin (PD4) to output and low
    DDRD |= 1 << DD4;
    PORTD &= ~(1 << PORT4);

    // set wake-up pin (PD3) to input and disable pullup
    DDRD &= ~(1 << DD3);
    PORTD &= ~(1 << PORT3);

    // enable pin change interrupt 2
    PCICR |= 1 << PCIE2;
    // make PD3 (PCINT19) contribute to pin change interrupt 2
    PCMSK2 |= 1 << PCINT19;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // sei does the same as
    // SREG |= 1 << SREG_I;
    sei();
    sleep_enable();
    while(1) {
        sleep_cpu();
    }
}

void respond_watch_dog()
{
    // send done signal to timer
    // -> prevents watch dog reset
    PORTD |= 1 << PORT4;
    _delay_ms(1);
    PORTD &= ~(1 << PORT4);
}

int main(void)
{
    // in case the watchdog reset the device keep it from resetting it again
    respond_watch_dog();

    // init
    flash_init();
    led_init();
    uart_init();

    set_led(1);
    _delay_ms(50);
    set_led(0);

    start_sleep();
}

// timer interrupt handler
// use pin change interrupt 1 on PD3
ISR(PCINT2_vect)
{
    // is PD3 high; so wake-up has been sent
    // ignore PD3 falling edge
    if(PIND & 1 << PIN3) {
        char    data[] = "How are you doing, my little flashy flash chip? You're really flashy, you know that. And you're doing a great job, keep up the good work!";
        uint8_t len    = strlen(data) + 1;
        // if(flash_sector_erase(0x0)) {
        //     uart_println("failed to erase");
        //     while(1)
        //         ;
        // }
        // uart_println(flash_write_data(0x69, data, len) ? "Write failed" : "Write succeeded");

        set_led(1);
        char buf[len];
        flash_read_data(0x69, buf, len);
        uart_println(buf);

        TempHum temp_hum;
        if(i2c_measure_temp_hum(&temp_hum))
            uart_println("Measurement failed!");
        char temp[6];
        char hum[6];
        utoa(temp_hum.temperature, temp, 10);
        utoa(temp_hum.humidity, hum, 10);
        uart_print("Temperature: ");
        uart_println(temp);
        uart_print("Humidity: ");
        uart_println(hum);
        uart_println("");
        uart_println("");
        uart_println("");
        set_led(0);

        respond_watch_dog();
    }
}
