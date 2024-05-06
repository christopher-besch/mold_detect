#pragma once

#include "flash_structs.h"

// return 0 on success
// return -1 on failure
int i2c_measure_temp_hum(FlashSensorData* sensor_data);
