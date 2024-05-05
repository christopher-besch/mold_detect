#pragma once

#include <stdint.h>

// to be run at boot up
void led_init();

void set_error_led(uint8_t state);

void set_atmosphere_led(uint8_t state);
