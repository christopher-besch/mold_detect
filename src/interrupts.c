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
//
// the 3V3 output of the usb controller is connected to the rest of the system via a diode
// the usb enable interrupt pin is connected to the usb controller side of the diode
// additionally the pin is pulled low using a resistor
// that way the pin gets pulled high when a usb host is attached
#define USB_ENABLE_DATA_DIRECTION_REGISTER DDRC
#define USB_ENABLE_PORT                    PORTC
#define USB_ENABLE_PIN                     PINC
#define USB_ENABLE_PIN_NR                  0

static uint8_t should_measure = 0;

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

    // set usb enable pin to input and disable pullup
    USB_ENABLE_DATA_DIRECTION_REGISTER &= ~(1 << USB_ENABLE_PIN_NR);
    USB_ENABLE_PORT &= ~(1 << USB_ENABLE_PIN_NR);

    // enable pin change interrupt 2
    PCICR |= 1 << PCIE2;
    // make wake pin PD3 (PCINT19) contribute to pin change interrupt 2
    PCMSK2 |= 1 << PCINT19;

    // enable pin change interrupt 1
    PCICR |= 1 << PCIE1;
    // make wake pin PC0 (PCINT8) contribute to pin change interrupt 1
    PCMSK1 |= 1 << PCINT8;
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
    WD_DONE_PORT |= 1 << WD_DONE_PIN_NR;
    _delay_ms(1);
    WD_DONE_PORT &= ~(1 << WD_DONE_PIN_NR);
}

void start_measurement_sleep_cycle()
{
    enable_measurements();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    while(1) {
        sleep_cpu();
    }
}

uint8_t is_usb_mode()
{
    return USB_ENABLE_PIN & (1 << USB_ENABLE_PIN_NR);
}

void enable_measurements()
{
    should_measure = 0;
}

void disable_measurements()
{
    should_measure = 0;
}

// timer interrupt handler
// use pin change interrupt 2 for PD3 (PCINT19)
ISR(PCINT2_vect)
{
    // is wake pin high; so wake-up has been sent
    // ignore wake pin falling edge
    if(WD_WAKE_PIN & (1 << WD_WAKE_PIN_NR)) {
        if(should_measure)
            perform_measurement();
        // always respond to the watchdog
        respond_watch_dog();
    }
}

// usb enable interrupt handler
// use pin change interrupt 1 for PC0 (PCINT8)
ISR(PCINT1_vect)
{
    if(is_usb_mode()) {
        set_usb_led(1);
        reset();
    }
    else {
        set_usb_led(0);
        reset();
    }
}
