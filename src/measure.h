#pragma once
#include <avr/sfr_defs.h>

void measure_perform_measurement();

// true iff the systems signals that the atmosphere is bad
// (i.e. the atmosphere led is on)
int measure_is_atmosphere_bad();

float measure_convert_temp_c(uint16_t raw);
float measure_convert_rel_hum(uint16_t raw);
