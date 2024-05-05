#include "measure.h"
#include "error.h"
#include "flash.h"
#include "ic2.h"
#include "interrupts.h"
#include "led.h"
#include "uart.h"
#include <stdlib.h>
#include <string.h>

void perform_measurement()
{
    uart_print("mold_detect (github.com/christopher-besch/mold_detect)\r\nversion: ");
    uart_println(MOLD_DETECT_VERSION);
    uart_println("Developed by Christopher Besch\r\nat the Chair for Embedded System at the KIT");

    char size_buf[6];
    utoa(sizeof(GenericFlashBlock), size_buf, 10);
    uart_print("sizeof(GenericFlashBlock): ");
    uart_println(size_buf);
    utoa(sizeof(FlashSensorData), size_buf, 10);
    uart_print("sizeof(FlashSensorData): ");
    uart_println(size_buf);
    utoa(sizeof(FlashTimestamp), size_buf, 10);
    uart_print("sizeof(FlashTimestamp): ");
    uart_println(size_buf);

    char    data[] = "This is pretty cool, is it not?";
    uint8_t len    = strlen(data) + 1;
    // if(flash_sector_erase(0x0)) {
    //     uart_println("failed to erase");
    //     while(1)
    //         ;
    // }
    // uart_println(flash_write_data(0x69, data, len) ? "Write failed" : "Write succeeded");

    set_atmosphere_led(1);
    char buf[len];
    if(flash_read_data(0x69, buf, len))
        uart_println("Flash read failed!");
    else
        uart_println(buf);

    TempHum temp_hum;
    if(i2c_measure_temp_hum(&temp_hum))
        uart_println("Measurement failed!");
    char temp[6];
    char hum[6];
    utoa(temp_hum.temperature, temp, 10);
    utoa(temp_hum.humidity, hum, 10);
    uart_print("Temperature: ");
    uart_println(temp);
    uart_print("Humidity: ");
    uart_println(hum);
    uart_println("");
    uart_println("");
    uart_println("");
    set_atmosphere_led(0);
}
