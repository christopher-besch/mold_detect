#include "error.h"
#include "led.h"
#include "uart.h"

#include <avr/eeprom.h>

#define ERROR_EEPROM_START_ADDR (uint8_t*)0x08

const char* get_error_msg(MoldError* error)
{
    const char* invalid_error_msg = "An invalid error type was raised.";

    switch(*error) {
    case MOLD_ERROR_INVALID_ERROR:
        return invalid_error_msg;
    case MOLD_ERROR_FAILED_TO_WRITE_BLOCK:
        return "Writing a block to the flash failed.";
    case MOLD_ERROR_FAILED_TO_RESET_ERROR_COUNTS:
        return "Resetting the error counts failed.";
    default:
        *error = MOLD_ERROR_INVALID_ERROR;
        return invalid_error_msg;
    }
}

uint8_t in_error_state()
{
    for(MoldError error = 0; error < MOLD_ERROR_NUM; ++error) {
        uint8_t* eeprom_addr = ERROR_EEPROM_START_ADDR + error;
        uint8_t  count       = eeprom_read_byte(eeprom_addr);

        if(count)
            return 1;
    }
    return 0;
}

void raise_error(MoldError error)
{
    const char* msg = get_error_msg(&error);
    uart_println(msg);

    uint8_t* eeprom_addr = ERROR_EEPROM_START_ADDR + error;
    uint8_t  old_count   = eeprom_read_byte(eeprom_addr);
    if(old_count != 0xff) {
        eeprom_write_byte(eeprom_addr, old_count + 1);
    }

    set_error_led(1);
}

void reset_errors()
{
    uart_print("Resetting all errors: ...");
    for(MoldError error = 0; error < MOLD_ERROR_NUM; ++error) {
        uint8_t* eeprom_addr = ERROR_EEPROM_START_ADDR + error;
        eeprom_update_byte(eeprom_addr, 0);
    }
    if(in_error_state())
        raise_error(MOLD_ERROR_FAILED_TO_RESET_ERROR_COUNTS);

    set_error_led(0);
    uart_println("done");
}

void list_errors()
{
    uart_println("# Occurred Errors #");
    for(MoldError error = 0; error < MOLD_ERROR_NUM; ++error) {
        uint8_t* eeprom_addr = ERROR_EEPROM_START_ADDR + error;
        uint8_t  count       = eeprom_read_byte(eeprom_addr);

        if(count == 0)
            continue;

        if(count == 0xff)
            uart_print(">=255: ");
        else {
            uart_print("  ");
            uart_print_uint8_t(count);
            uart_print(": ");
        }
        uart_println(get_error_msg(&error));
    }
    uart_println("# End of Occurred Errors #");
}
