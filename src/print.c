#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "flash_blocks.h"
#include "measure.h"
#include "print.h"
#include "uart.h"

static char utoa_buf[5];

// use kit_logo_encoding.py to calculate
static const uint8_t kit_logo_run_length_encoding[] = {16, 5, 8, 7, 2, 6, 1, 14, 10, 3, 4, 4, 6, 7, 4, 6, 1, 14, 9, 5, 3, 4, 4, 8, 5, 6, 5, 6, 14, 5, 3, 3, 3, 7, 7, 6, 5, 6, 8, 3, 5, 4, 2, 3, 2, 6, 9, 6, 5, 6, 7, 6, 4, 4, 2, 2, 2, 6, 9, 6, 5, 6, 8, 8, 3, 3, 1, 2, 2, 7, 8, 6, 5, 6, 13, 5, 3, 2, 1, 1, 4, 7, 6, 6, 5, 6, 5, 4, 8, 3, 2, 1, 1, 1, 6, 7, 4, 6, 5, 6, 4, 13, 4, 2, 10, 6, 3, 6, 5, 6, 4, 21, 9, 7, 1, 6, 5, 6, 4};
static const uint8_t kit_logo_width                 = 59;
static const char    kit_logo_first_char            = ' ';
static const char    kit_logo_other_char            = '#';

void uart_print_uint8_t_dec(uint8_t val)
{
    utoa(val, utoa_buf, 10);
    uart_print(utoa_buf);
}

void uart_print_hex_digit(uint8_t val)
{
    if(val < 0xa)
        uart_trans('0' - 0x0 + val);
    else if(val < 0x10)
        uart_trans('a' - 0xa + val);
    else {
        raise_fatal_error(MOLD_ERROR_INVALID_PARAMS_UART_PRINT_HEX_DIGIT_MORE_THAN_ONE_DIGIT);
    }
}
void uart_print_uint8_t_hex(uint16_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_uint16_t_hex(uint16_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x0c) & 0x0f);
    uart_print_hex_digit((val >> 0x08) & 0x0f);
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_uint32_t_hex(uint32_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x1c) & 0x0f);
    uart_print_hex_digit((val >> 0x18) & 0x0f);
    uart_print_hex_digit((val >> 0x14) & 0x0f);
    uart_print_hex_digit((val >> 0x10) & 0x0f);
    uart_print_hex_digit((val >> 0x0c) & 0x0f);
    uart_print_hex_digit((val >> 0x08) & 0x0f);
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_uint64_t_hex(uint64_t val)
{
    uart_print("0x");
    uart_print_hex_digit((val >> 0x3c) & 0x0f);
    uart_print_hex_digit((val >> 0x38) & 0x0f);
    uart_print_hex_digit((val >> 0x34) & 0x0f);
    uart_print_hex_digit((val >> 0x30) & 0x0f);
    uart_print_hex_digit((val >> 0x2c) & 0x0f);
    uart_print_hex_digit((val >> 0x28) & 0x0f);
    uart_print_hex_digit((val >> 0x24) & 0x0f);
    uart_print_hex_digit((val >> 0x20) & 0x0f);
    uart_print_hex_digit((val >> 0x1c) & 0x0f);
    uart_print_hex_digit((val >> 0x18) & 0x0f);
    uart_print_hex_digit((val >> 0x14) & 0x0f);
    uart_print_hex_digit((val >> 0x10) & 0x0f);
    uart_print_hex_digit((val >> 0x0c) & 0x0f);
    uart_print_hex_digit((val >> 0x08) & 0x0f);
    uart_print_hex_digit((val >> 0x04) & 0x0f);
    uart_print_hex_digit((val >> 0x00) & 0x0f);
}
void uart_print_float(float val)
{
    char buf[20];
    MD_ASSERT(sprintf(buf, "%f", val) != 0, MOLD_ERROR_UART_PRINT_FLOAT_SPRINTF_FAILED);
    uart_print(buf);
}
void uart_print_bool(uint8_t val)
{
    uart_print(val ? "true" : "false");
}

// expected_timestamp might be NULL
void flash_print_sensor_data_block(FlashSensorData* block, uint64_t* expected_timestamp)
{
    MD_ASSERT(block, MOLD_ERROR_FLASH_PRINT_SENSOR_DATA_BLOCK_NULL);
    uart_print("\"raw\":\"");
    uart_print_uint64_t_hex(*(uint64_t*)block);
    uart_print("\",\"type\":\"sensdat\"");
    if(expected_timestamp) {
        uart_print(",\"timestamp\":\"");
        uart_print_uint64_t_hex(*expected_timestamp);
        uart_print("\"");
    }
    uart_print(",\"free\":");
    uart_print_bool(flash_is_block_free(block->flags));
    uart_print(",\"atmos_bad\":");
    uart_print_bool(flash_is_block_atmos_bad(block->flags));
    uart_print(",\"err_set\":");
    uart_print_bool(flash_is_block_err_set(block->flags));
    uart_print(",\"temp\":");
    uart_print_float(measure_convert_temp_c(block->temperature));
    uart_print(",\"hum\":");
    uart_print_float(measure_convert_rel_hum(block->humidity));
    uart_print(",\"traw\":\"");
    uart_print_uint16_t_hex(block->temperature);
    uart_print("\",\"traw\":\"");
    uart_print_uint16_t_hex(block->humidity);
    uart_print("\",\"tcrc\":\"");
    uart_print_uint16_t_hex(block->temperature_crc);
    uart_print("\",\"hcrc\":\"");
    uart_print_uint16_t_hex(block->humidity_crc);
    uart_print("\"");
}
// expected_timestamp might be NULL
void flash_print_timestamp_block(FlashTimestamp* block, uint64_t* expected_timestamp)
{
    MD_ASSERT(block, MOLD_ERROR_FLASH_PRINT_TIMESTAMP_BLOCK_NULL);
    uart_print("\"raw\":\"");
    uart_print_uint64_t_hex(*(uint64_t*)block);
    uart_print("\",\"type\":\"tmstmp\"");
    uart_print(",\"free\":");
    uart_print_bool(flash_is_block_free(block->flags));
    uart_print(",\"atmos_bad\":");
    uart_print_bool(flash_is_block_atmos_bad(block->flags));
    uart_print(",\"err_set\":");
    uart_print_bool(flash_is_block_err_set(block->flags));
    uart_print(",\"timestamp\":\"");
    uart_print_uint64_t_hex(block->unix_second_timestamp);
    uart_print("\"");
    // don't update NULL
    if(expected_timestamp)
        *expected_timestamp = block->unix_second_timestamp;
}
// expected_timestamp might be NULL
void uart_print_flash_block(GenericFlashBlock* block, uint64_t* expected_timestamp)
{
    MD_ASSERT(block, MOLD_ERROR_UART_PRINT_FLASH_BLOCK_NULL);

    uart_print("{");
    switch(flash_get_block_type(block->flags)) {
    case SENSOR_DATA_BLOCK:
        flash_print_sensor_data_block((FlashSensorData*)block, expected_timestamp);
        break;
    case TIMESTAMP_BLOCK:
        flash_print_timestamp_block((FlashTimestamp*)block, expected_timestamp);
        break;
    default:
        raise_fatal_error(MOLD_ERROR_UART_PRINT_FLASH_BLOCK_INVALID_TYPE);
    }
    uart_print("}");
}

void uart_print_kit_logo()
{
    char     cur_char        = kit_logo_first_char;
    uint16_t cur_pos_in_line = 0;
    uart_println("");
    uart_println("");
    uart_println("");
    uart_println("");
    uart_print("    ");
    // loop over run length encoding
    for(uint8_t i = 0; i < sizeof(kit_logo_run_length_encoding); ++i) {
        for(uint8_t j = 0; j < kit_logo_run_length_encoding[i]; ++j) {
            // make sure we add the correct line breaks
            if(cur_pos_in_line == kit_logo_width) {
                uart_print("\r\n    ");
                cur_pos_in_line = 0;
            }
            uart_trans(cur_char);
            ++cur_pos_in_line;
        }
        // go to other char
        cur_char = cur_char == kit_logo_first_char ? kit_logo_other_char : kit_logo_first_char;
    }
    uart_println("");
    uart_println("");
    uart_println("");
    uart_println("");
}
