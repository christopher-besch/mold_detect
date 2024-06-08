#include "measure.h"
#include "error.h"
#include "flash.h"
#include "i2c.h"
#include "led.h"
#include "uart.h"
#include <util/delay.h>

static uint8_t atmosphere_bad = 0;

void perform_measurement()
{
    // TODO: remove debug led here
    set_atmosphere_led(1);

    static FlashSensorData sensor_data;
    MD_ASSERT(!i2c_measure_temp_hum(&sensor_data), MOLD_ERROR_PERFORM_MEASUREMENT_FAILED);

    if(!flash_is_full())
        flash_write_next_block((GenericFlashBlock*)&sensor_data);
    else
        uart_println("Warning: don't store measurement as the flash is full");

    set_atmosphere_led(atmosphere_bad);
}

int is_atmosphere_bad()
{
    return atmosphere_bad;
}

float convert_temp_c(uint16_t raw)
{
    // -45 + 175 * raw / (2**16-1)
    return -45.0f + 0.002670328831921874f * (float)raw;
}
float convert_rel_hum(uint16_t raw)
{
    // 100 * raw / (2**16-1)
    return 0.0015259021896696422f * (float)raw;
}
