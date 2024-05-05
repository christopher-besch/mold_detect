#include "interrupts.h"
#include "measure.h"
#include "uart.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

void interrupt_init()
{
    // clear status register
    MCUSR = 0;
    wdt_disable();
    respond_watch_dog();

    // set watchdog respond (done) pin (PD4) to output and low
    DDRD |= 1 << DD4;
    PORTD &= ~(1 << PORT4);

    // set wake-up pin (PD3) to input and disable pullup
    DDRD &= ~(1 << DD3);
    PORTD &= ~(1 << PORT3);

    // enable pin change interrupt 2
    PCICR |= 1 << PCIE2;
    // make PD3 (PCINT19) contribute to pin change interrupt 2
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
    PORTD |= 1 << PORT4;
    _delay_ms(1);
    PORTD &= ~(1 << PORT4);
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

// timer interrupt handler
// use pin change interrupt 1 on PD3
ISR(PCINT2_vect)
{
    // is PD3 high; so wake-up has been sent
    // ignore PD3 falling edge
    if(PIND & 1 << PIN3) {
        perform_measurement();
    }
}
