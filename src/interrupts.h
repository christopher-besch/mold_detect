#pragma once

#include <avr/sfr_defs.h>

void interrupt_init();

// reset the system
void reset();

void respond_watch_dog();

void start_measurement_sleep_cycle();

uint8_t is_usb_mode();

void enable_measurements();
void disable_measurements();
