#pragma once

#include <avr/sfr_defs.h>

void interrupt_init();

// reset the system
void reset();

void respond_watch_dog();

void start_measurement_sleep_cycle();

// is the usb port currently attached to a usb host?
uint8_t is_usb_mode();

// when the watchdog interrupt hits, should we perform a measurement?
// otherwise the watchdog simply gets immediately responded
void enable_measurements();
void disable_measurements();
