#pragma once

#include <stdint.h>

// There is one uint8_t for each error type in the atmega eeprom.
// They contain the count for that specific error type.
typedef enum _moldError {
    MOLD_ERROR_INVALID_ERROR                                                  = 0x00,
    MOLD_ERROR_FAILED_TO_RESET_ERROR_COUNTS                                   = 0x01,
    MOLD_ERROR_INVALID_PARAMS_NULL_TERMINATE_AFTER_FIRST_WORD                 = 0x02,
    MOLD_ERROR_INVALID_PARAMS_PARSE_ERROR_SUBCMD                              = 0x03,
    MOLD_ERROR_INVALID_PARAMS_PARSE_CMD                                       = 0x04,
    MOLD_ERROR_INVALID_PARAMS_PARSE_FLASH_SUBCMD                              = 0x05,
    MOLD_ERROR_INVALID_PARAMS_FLASH_IS_SECTOR_ERASED_NOT_AT_START_OF_SECTOR   = 0x06,
    MOLD_ERROR_INVALID_PARAMS_FLASH_IS_DATA_VALID_DATA_IS_NULL                = 0x07,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_BUF_IS_NULL                     = 0x0b,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_DATA_ADDRESS_OR_NBYTES_IS_TOO_HIGH   = 0x0c,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_DATA_DATA_IS_NULL                   = 0x0d,
    MOLD_ERROR_FLASH_WRITE_DATA_VALIDATION_FAILED                             = 0x0e,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_BLOCK_IS_NULL                  = 0x0f,
    MOLD_ERROR_INVALID_PARAMS_FLASH_READ_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK  = 0x10,
    MOLD_ERROR_INVALID_PARAMS_IS_BLOCK_FREE_ADDRESS_DOESNT_START_AT_BLOCK     = 0x11,
    MOLD_ERROR_INVALID_PARAMS_UART_PRINT_HEX_DIGIT_MORE_THAN_ONE_DIGIT        = 0x12,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_BLOCK_ADDRESS_DOESNT_START_AT_BLOCK = 0x13,
    MOLD_ERROR_FLASH_WRITE_NEXT_BLOCK_FLASH_IS_FULL                           = 0x14,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_NEXT_BLOCK_BLOCK_IS_NULL            = 0x15,
    MOLD_ERROR_INVALID_PARAMS_FLASH_WRITE_BLOCK_ADDRESS_BLOCK_IS_NULL         = 0x16,
    MOLD_ERROR_FLASH_CHIP_ERASE_FAILED                                        = 0x17,
    MOLD_ERROR_FLASH_WRITE_NEXT_BLOCK_FIND_NEXT_FREE_BLOCK_FAILED             = 0x18,
    MOLD_ERROR_FLASH_FLASH_CHIP_ERASE_FIND_NEXT_FREE_BLOCK_FAILED             = 0x19,

    MOLD_ERROR_TEST,

    // how many errors are there?
    MOLD_ERROR_NUM,
} MoldError;

void raise_error(MoldError error);

void reset_errors();

void list_errors();

uint8_t in_error_state();
