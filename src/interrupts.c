#include "interrupts.h"
#include "led.h"
#include "measure.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

// this pin is pulled high when the tpl5010 times out
#define WD_WAKE_DATA_DIRECTION_REGISTER DDRD
#define WD_WAKE_PORT                    PORTD
#define WD_WAKE_PIN                     PIND
#define WD_WAKE_PIN_NR                  3

// this pin needs to be pulled high before the next
// wake up interrupt
// otherwise the tpl5010 performs a reset
#define WD_DONE_DATA_DIRECTION_REGISTER DDRD
#define WD_DONE_PORT                    PORTD
#define WD_DONE_PIN                     PIND
#define WD_DONE_PIN_NR                  4

// this pin is connected to the power output of the uart to usb controller
// that way it is low when no usb host is connected and high otherwise
#define USB_ENABLE_DONE_DATA_DIRECTION_REGISTER DDRC
#define USB_ENABLE_DONE_PORT                    PORTC
#define USB_ENABLE_DONE_PIN                     PINC
#define USB_ENABLE_DONE_PIN_NR                  0

void interrupt_init()
{
    // clear status register
    MCUSR = 0;
    wdt_disable();
    respond_watch_dog();

    // set watchdog respond pin to output and low
    WD_DONE_DATA_DIRECTION_REGISTER |= 1 << WD_DONE_PIN_NR;
    WD_DONE_PORT &= ~(1 << WD_DONE_PIN_NR);

    // set wake-up pin to input and disable pullup
    WD_WAKE_DATA_DIRECTION_REGISTER &= ~(1 << WD_WAKE_PIN_NR);
    WD_WAKE_PORT &= ~(1 << WD_WAKE_PIN_NR);

    // enable pin change interrupt 2
    PCICR |= 1 << PCIE2;
    // make wake pin PD3 (PCINT19) contribute to pin change interrupt 2
    PCMSK2 |= 1 << PCINT19;
}

void reset()
{
    uart_println("Performing a software reset");
    // enable avr watchdog and wait for it to hit
    wdt_enable(WDTO_15MS);
    while(1)
        ;
}

void respond_watch_dog()
{
    // send done signal to timer
    // -> prevents watch dog reset
    PORTD |= 1 << WD_DONE_PIN_NR;
    _delay_ms(1);
    PORTD &= ~(1 << WD_DONE_PIN_NR);
}

void start_measurement_sleep_cycle()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // sei does the same as
    // SREG |= 1 << SREG_I;
    sei();
    sleep_enable();
    while(1) {
        sleep_cpu();
    }
}

void is_usb_mode()
{
}

// timer interrupt handler
// use pin change interrupt 2 for PD3 (PCINT19)
ISR(PCINT2_vect)
{
    // is wake pin high; so wake-up has been sent
    // ignore wake pin falling edge
    if(WD_WAKE_PIN & 1 << WD_WAKE_PIN_NR) {
        perform_measurement();
    }
}

// timer interrupt handler
// use pin change interrupt 2 for PC0 (PCINT8)
ISR(PCINT1_vect)
{
    set_usb_led(1);
    _delay_ms(50);
    set_usb_led(0);
}
