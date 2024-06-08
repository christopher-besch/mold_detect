#pragma once
#include <avr/sfr_defs.h>

void perform_measurement();

// true iff the systems signals that the atmosphere is bad
// (i.e. the atmosphere led is on)
int is_atmosphere_bad();

float convert_temp_c(uint16_t raw);
float convert_rel_hum(uint16_t raw);
