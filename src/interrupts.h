#pragma once

void interrupt_init();

// reset the system
void reset();

void respond_watch_dog();

void start_measurement_sleep_cycle();

void is_usb_mode();
