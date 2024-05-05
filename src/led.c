#include "led.h"
#include "error.h"

#include <avr/io.h>

#define ERROR_LED_DATA_DIRECTION_REGISTER DDRD
#define ERROR_LED_PORT                    PORTD
#define ERROR_LED_PIN                     6

#define ATMOSPHERE_LED_DATA_DIRECTION_REGISTER DDRD
#define ATMOSPHERE_LED_PORT                    PORTD
#define ATMOSPHERE_LED_PIN                     2

void led_init()
{
    ERROR_LED_DATA_DIRECTION_REGISTER |= 1 << ERROR_LED_PIN;
    set_error_led(in_error_state());
    ATMOSPHERE_LED_DATA_DIRECTION_REGISTER |= 1 << ATMOSPHERE_LED_PIN;
    set_atmosphere_led(0);
}

void set_error_led(uint8_t state)
{
    if(state)
        ERROR_LED_PORT |= 1 << ERROR_LED_PIN;
    else
        ERROR_LED_PORT &= ~(1 << ERROR_LED_PIN);
}

void set_atmosphere_led(uint8_t state)
{
    if(state)
        ATMOSPHERE_LED_PORT |= 1 << ATMOSPHERE_LED_PIN;
    else
        ATMOSPHERE_LED_PORT &= ~(1 << ATMOSPHERE_LED_PIN);
}
