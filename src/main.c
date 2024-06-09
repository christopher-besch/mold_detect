#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "flash.h"
#include "i2c.h"
#include "interrupts.h"
#include "led.h"
#include "terminal.h"
#include "uart.h"

void set_clock_speed()
{
    // see atmega328p docs 13.12.2 (p.60)
#if CLOCK_DIV == 1
    uint8_t clkps_3_0 = 0b0000;
#endif
#if CLOCK_DIV == 2
    uint8_t clkps_3_0 = 0b0001;
#endif
#if CLOCK_DIV == 4
    uint8_t clkps_3_0 = 0b0010;
#endif
#if CLOCK_DIV == 8
    uint8_t clkps_3_0 = 0b0011;
#endif
#if CLOCK_DIV == 16
    uint8_t clkps_3_0 = 0b0100;
#endif
#if CLOCK_DIV == 32
    uint8_t clkps_3_0 = 0b0101;
#endif
#if CLOCK_DIV == 64
    uint8_t clkps_3_0 = 0b0110;
#endif
#if CLOCK_DIV == 128
    uint8_t clkps_3_0 = 0b0111;
#endif
#if CLOCK_DIV == 256
    uint8_t clkps_3_0 = 0b1000;
#endif
    // enable updating the clock speed
    CLKPR = 1 << CLKPCE;
    // divide clock by whatever the preprocessor calculated
    CLKPR = clkps_3_0 << CLKPS0;
}

int main(void)
{
    set_clock_speed();

    // init //
    uart_init();
    // this should go as one of the first as the avr wdt might still be enabled
    interrupt_init();
    flash_init();
    led_init();
    i2c_init();

    set_atmosphere_led(1);
    uart_println("");
    uart_println("mold_detect booting up");
    print_version();
    _delay_ms(50);
    set_atmosphere_led(0);

    sei();
    // if you want uart for debugging, just enter the sleep cycle every time
    if(is_usb_mode())
        enter_terminal();
    else
        start_measurement_sleep_cycle();
}
