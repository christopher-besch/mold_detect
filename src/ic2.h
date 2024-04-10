#pragma once

#include <avr/sfr_defs.h>
typedef struct _tempHum {
    uint16_t temperature;
    uint16_t humidity;
} TempHum;

// return 0 on success
// return -1 on failure
int i2c_measure_temp_hum(TempHum* temp_hum);
