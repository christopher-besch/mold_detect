#include "measure.h"
#include "error.h"
#include "flash.h"
#include "flash_blocks.h"
#include "i2c.h"
#include "led.h"
#include "uart.h"
#include <util/delay.h>

static uint8_t atmosphere_bad = 0;

void measure_perform_measurement()
{
    static FlashSensorData sensor_data;
    if(!i2c_measure_temp_hum(&sensor_data)) {
        // Don't reset to create and then store the block
        // This stores when the error occured and keeps the time up to date.
        raise_error(MOLD_ERROR_PERFORM_MEASUREMENT_FAILED);
        flash_create_sensor_data_block(&sensor_data, 0, 0, 0, 0);
    }

    if(!flash_is_full())
        flash_write_next_block((GenericFlashBlock*)&sensor_data);
    else
        uart_println("Warning: don't store measurement as the flash is full");

    set_atmosphere_led(atmosphere_bad);
}

int measure_is_atmosphere_bad()
{
    return atmosphere_bad;
}

float measure_convert_temp_c(uint16_t raw)
{
    // -45 + 175 * raw / (2**16-1)
    return -45.0f + 0.002670328831921874f * (float)raw;
}
float measure_convert_rel_hum(uint16_t raw)
{
    // 100 * raw / (2**16-1)
    return 0.0015259021896696422f * (float)raw;
}
