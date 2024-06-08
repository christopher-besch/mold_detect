#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "flash.h"
#include "interrupts.h"
#include "led.h"
#include "terminal.h"
#include "uart.h"

void set_clock_speed()
{
    // enable updating the clock speed
    CLKPR = 1 << CLKPCE;
    // divide clock by 2
    CLKPR = 0b001 << CLKPS0;
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
