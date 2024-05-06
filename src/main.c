#include <avr/interrupt.h>
#include <util/delay.h>

#include "flash.h"
#include "interrupts.h"
#include "led.h"
#include "terminal.h"
#include "uart.h"

int main(void)
{
    // init
    // this should go first as the avr wdt might still be enabled
    interrupt_init();
    flash_init();
    led_init();
    uart_init();

    set_atmosphere_led(1);
    uart_println("");
    uart_println("");
    uart_println("mold_detect booting up");
    _delay_ms(50);
    set_atmosphere_led(0);

    sei();
    // if you want uart for debugging, just enter the sleep cycle every time
    // if(is_usb_mode())
    //     enter_terminal();
    // else
    start_measurement_sleep_cycle();
}
