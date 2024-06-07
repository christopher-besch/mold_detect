#pragma once

#include "flash_blocks.h"

// Ask the sensor about the current atmosphere condition.
// Store those values in sensor_data.
// return 0 on success
// return -1 on failure
int i2c_measure_temp_hum(FlashSensorData* sensor_data);
