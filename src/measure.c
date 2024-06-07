#include "measure.h"
#include "flash.h"
#include "i2c.h"
#include "led.h"
#include "uart.h"
#include <stdlib.h>
#include <util/delay.h>

static uint8_t atmosphere_bad = 0;

void perform_measurement()
{
    // TODO: remove debug led here
    set_atmosphere_led(1);

    FlashSensorData sensor_data;
    if(i2c_measure_temp_hum(&sensor_data))
        uart_println("Measurement failed!");

    flash_write_next_block((GenericFlashBlock*)&sensor_data);

    set_atmosphere_led(atmosphere_bad);
}

int is_atmosphere_bad()
{
    return atmosphere_bad;
}
