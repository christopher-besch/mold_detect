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
    char temp[6];
    char hum[6];
    utoa(sensor_data.temperature, temp, 10);
    utoa(sensor_data.humidity, hum, 10);
    uart_print("Temperature: ");
    uart_println(temp);
    uart_print("Humidity: ");
    uart_println(hum);

    flash_write_next_block((GenericFlashBlock*)&sensor_data);

    set_atmosphere_led(atmosphere_bad);
}

int is_atmosphere_bad()
{
    return atmosphere_bad;
}
