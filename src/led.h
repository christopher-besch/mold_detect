#pragma once

#include <stdint.h>

// to be run at boot up
void led_init();

// using LED Blue1
void set_error_led(uint8_t state);

// using LED Red2
void set_atmosphere_led(uint8_t state);

// using LED Green1
void set_usb_led(uint8_t state);
