#pragma once

#include <stdint.h>

#define MD_ASSERT(cond, err)        \
    do {                            \
        if(!(cond)) {               \
            raise_fatal_error(err); \
        }                           \
    } while(0)

// There is one uint8_t for each error type in the atmega eeprom.
// They contain the count for that specific error type.
typedef enum _moldError {
    MOLD_ERROR_INVALID_ERROR                                                   = 0x00,
    MOLD_ERROR_FAILED_TO_RESET_ERROR_COUNTS                                    = 0x01,
    MOLD_ERROR_INVALID_PARAMS_NULL_TERMINATE_AFTER_FIRST_WORD                  = 0x02,
    MOLD_ERROR_INVALID_PARAMS_PARSE_ERROR_SUBCMD                               = 0x03,
    MOLD_ERROR_INVALID_PARAMS_PARSE_CMD                                        = 0x04,
    MOLD_ERROR_INVALID_PARAMS_PARSE_FLASH_SUBCMD                               = 0x05,
    MOLD_ERROR_INVALID_PARAMS_FLASH_IS_SECTOR_ERASED_NOT_AT_START_OF_SECTOR    = 0x06,
    MOLD_ERROR_INVALID_PARAMS_FLASH_IS_DATA_VALID_DATA_IS_NULL                 = 0x07,
    MOLD_ERROR_FLASH_CHECK_CORRECT_FREE_BLOCK_ADDR_NEXT_FREE_BLOCK_NOT_FREE    = 0x08,
    MOLD_ERROR_FLASH_CHECK_CORRECT_FREE_BLOCK_ADDR_BEFORE_NEXT_FREE_BLOCK_FREE = 0x09,
    MOLD_ERROR_UART_PRINT_FLOAT_SPRINTF_FAILED                                 = 0x0a,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_BUF_IS_NULL                      = 0x0b,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_ADDRESS_OR_NBYTES_IS_TOO_HIGH    = 0x0c,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_DATA_DATA_IS_NULL                    = 0x0d,
    MOLD_ERROR_FLASH_WRITE_DATA_VALIDATION_FAILED                              = 0x0e,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_BLOCK_IS_NULL                   = 0x0f,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK   = 0x10,
    MOLD_ERROR_INVALID_PARAMS_IS_BLOCK_FREE_ADDRESS_DOESNT_START_AT_BLOCK      = 0x11,
    MOLD_ERROR_INVALID_PARAMS_UART_PRINT_HEX_DIGIT_MORE_THAN_ONE_DIGIT         = 0x12,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK  = 0x13,
    MOLD_ERROR_FLASH_WRITE_NEXT_BLOCK_FLASH_IS_FULL                            = 0x14,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_NEXT_BLOCK_BLOCK_IS_NULL             = 0x15,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_BLOCK_ADDRESS_BLOCK_IS_NULL          = 0x16,
    MOLD_ERROR_FLASH_CHIP_ERASE_FAILED                                         = 0x17,
    MOLD_ERROR_FLASH_WRITE_NEXT_BLOCK_FIND_NEXT_FREE_BLOCK_FAILED              = 0x18,
    MOLD_ERROR_FLASH_FLASH_CHIP_ERASE_FIND_NEXT_FREE_BLOCK_FAILED              = 0x19,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_HUM_CRC                          = 0x1a,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_SEND_STOP                        = 0x1b,
    MD_ERROR_I2C_MEASURE_TEMP_HUM_ERROR_STOP_FAILED                            = 0x1c,
    MOLD_ERROR_UART_PRINT_FLASH_BLOCK_NULL                                     = 0x1d,
    MOLD_ERROR_FLASH_SET_BLOCK_FLAGS_NULL                                      = 0x1e,
    MOLD_ERROR_FLASH_CREATE_SENSOR_DATA_BLOCK_NULL                             = 0x1f,
    MOLD_ERROR_FLASH_CREATE_TIMESTAMP_BLOCK_NULL                               = 0x20,
    MOLD_ERROR_FLASH_PRINT_TIMESTAMP_BLOCK_NULL                                = 0x21,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_HUM0                             = 0x22,
    MOLD_ERROR_UART_PRINT_FLASH_BLOCK_INVALID_TYPE                             = 0x23,
    MOLD_ERROR_FLASH_GET_BLOCK_TYPE_INVALID_TYPE                               = 0x24,
    MOLD_ERROR_FLASH_PRINT_SENSOR_DATA_BLOCK_NULL                              = 0x25,
    MOLD_ERROR_INVALID_PARAMS_PARSE_TIME_SUBCMD                                = 0x26,
    MOLD_ERROR_PERFORM_MEASUREMENT_FAILED                                      = 0x27,
    MD_ERROR_SET_TIMESTAMP_NULL                                                = 0x28,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_HUM1                             = 0x29,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_STOP_FAILED                          = 0x2a,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_START2_FAILED                             = 0x2b,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_ADDR_FAILED                      = 0x2c,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_TEMP1                            = 0x2d,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_TEMP0                            = 0x2e,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_REC_TEMP_CRC                         = 0x2f,
    MD_ERROR_I2C_START_CHECK_STATUS_FAILED                                     = 0x30,
    MD_ERROR_I2C_SEND_CHECK_STATUS_FAILED                                      = 0x31,
    MD_ERROR_I2C_RECEIVE_CHECK_STATUS_FAILED                                   = 0x32,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_START1_FAILED                             = 0x33,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_ADDR_FAILED                          = 0x34,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_MSB_CMD_FAILED                       = 0x35,
    MD_ERROR_I2C_ATTEMPT_READ_SENSOR_SEND_LSB_CMD_FAILED                       = 0x36,

    MOLD_ERROR_TEST,

    // how many errors are there?
    MOLD_ERROR_NUM,
} MoldError;

void raise_error(MoldError error);
// raise error and then reset the device
void raise_fatal_error(MoldError error);

void reset_errors();

void list_errors();

uint8_t in_error_state();
