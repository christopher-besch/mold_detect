#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "error.h"
#include "flash.h"
#include "ic2.h"
#include "led.h"
#include "uart.h"

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

    set_atmosphere_led(1);

    uart_println("");
    uart_println("");
    uart_println("");
    uart_println("mold_detect booting up");
    uart_println("");
    uart_println("");
    uart_println("");
    list_errors();

    _delay_ms(50);
    set_atmosphere_led(0);

    start_sleep();
}

// timer interrupt handler
// use pin change interrupt 1 on PD3
ISR(PCINT2_vect)
{
    // is PD3 high; so wake-up has been sent
    // ignore PD3 falling edge
    if(PIND & 1 << PIN3) {
        uart_print("mold_detect (github.com/christopher-besch/mold_detect)\r\nversion: ");
        uart_println(MOLD_DETECT_VERSION);
        uart_println("Developed by Christopher Besch\r\nat the Chair for Embedded System at the KIT");

        char size_buf[6];
        utoa(sizeof(GenericFlashBlock), size_buf, 10);
        uart_print("sizeof(GenericFlashBlock): ");
        uart_println(size_buf);
        utoa(sizeof(FlashSensorData), size_buf, 10);
        uart_print("sizeof(FlashSensorData): ");
        uart_println(size_buf);
        utoa(sizeof(FlashTimestamp), size_buf, 10);
        uart_print("sizeof(FlashTimestamp): ");
        uart_println(size_buf);

        char    data[] = "This is pretty cool, is it not?";
        uint8_t len    = strlen(data) + 1;
        // if(flash_sector_erase(0x0)) {
        //     uart_println("failed to erase");
        //     while(1)
        //         ;
        // }
        // uart_println(flash_write_data(0x69, data, len) ? "Write failed" : "Write succeeded");

        set_atmosphere_led(1);
        char buf[len];
        if(flash_read_data(0x69, buf, len))
            uart_println("Flash read failed!");
        else
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
        set_atmosphere_led(0);

        respond_watch_dog();
    }
}
