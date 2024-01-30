#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

int main(void)
{
    // set DDD2 to output
    DDRD |= 1 << DD2;
    // set DDB1 to input
    DDRB &= ~(1 << DD1);
    // enable pullup
    // PUD is not set
    PORTB |= 1 << PORT1;

    // enable pin change interrupts 0
    PCICR |= 1 << PCIE0;
    // enable PORTB1 for pin change interrupt
    PCMSK0 |= 1 << PCINT1;

    // set PORTD2 to high
    PORTD |= 1 << PORT2;

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // interrupt enabled part
    {
        sei();
        // sei does the same as
        // SREG |= 1 << SREG_I;
        sleep_enable();
        while(1) {
            sleep_cpu();
        }
        sleep_disable();
        cli();
    }
}

// button press handler
// use pin change interrupt 0 on PORTB1
ISR(PCINT0_vect)
{
    // is PINB1 high; so button not pressed?
    if(PINB & 1 << PIN1) {
        // set PORTD2 to high
        PORTD |= 1 << PORT2;
    }
    else {
        // set PORTD2 to low
        PORTD &= ~(1 << PORT2);
    }
}

// resulting current draw at 5V:
// 0.47mA with LED off
// 9mA with LED on
