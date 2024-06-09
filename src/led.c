#include "led.h"
#include "error.h"
#include "interrupts.h"
#include "measure.h"

#include <avr/io.h>

// using LED Blue1
#define ERROR_LED_DATA_DIRECTION_REGISTER DDRD
#define ERROR_LED_PORT                    PORTD
#define ERROR_LED_PIN                     PIND
#define ERROR_LED_PIN_NR                  6

// using LED Red2
#define ATMOSPHERE_LED_DATA_DIRECTION_REGISTER DDRD
#define ATMOSPHERE_LED_PORT                    PORTD
#define ATMOSPHERE_LED_PIN                     PIND
#define ATMOSPHERE_LED_PIN_NR                  2

// using LED Green1
#define USB_LED_DATA_DIRECTION_REGISTER DDRD
#define USB_LED_PORT                    PORTD
#define USB_LED_PIN                     PIND
#define USB_LED_PIN_NR                  5

// using LED Red1
#define GENERAL_LED_DATA_DIRECTION_REGISTER DDRD
#define GENERAL_LED_PORT                    PORTD
#define GENERAL_LED_PIN                     PIND
#define GENERAL_LED_PIN_NR                  7

void led_init()
{
    ERROR_LED_DATA_DIRECTION_REGISTER |= 1 << ERROR_LED_PIN_NR;
    set_error_led(in_error_state());
    ATMOSPHERE_LED_DATA_DIRECTION_REGISTER |= 1 << ATMOSPHERE_LED_PIN_NR;
    set_atmosphere_led(measure_is_atmosphere_bad());
    USB_LED_DATA_DIRECTION_REGISTER |= 1 << USB_LED_PIN_NR;
    set_usb_led(is_usb_mode());
    GENERAL_LED_DATA_DIRECTION_REGISTER |= 1 << GENERAL_LED_PIN_NR;
    set_general_led(0);
}

void set_error_led(uint8_t state)
{
    if(state)
        ERROR_LED_PORT |= 1 << ERROR_LED_PIN_NR;
    else
        ERROR_LED_PORT &= ~(1 << ERROR_LED_PIN_NR);
}

void set_atmosphere_led(uint8_t state)
{
    if(state)
        ATMOSPHERE_LED_PORT |= 1 << ATMOSPHERE_LED_PIN_NR;
    else
        ATMOSPHERE_LED_PORT &= ~(1 << ATMOSPHERE_LED_PIN_NR);
}

void set_usb_led(uint8_t state)
{
    if(state)
        USB_LED_PORT |= 1 << USB_LED_PIN_NR;
    else
        USB_LED_PORT &= ~(1 << USB_LED_PIN_NR);
}

void set_general_led(uint8_t state)
{
    if(state)
        GENERAL_LED_PORT |= 1 << GENERAL_LED_PIN_NR;
    else
        GENERAL_LED_PORT &= ~(1 << GENERAL_LED_PIN_NR);
}
