#pragma once

#include <stdint.h>

typedef enum _moldError {
    MOLD_ERROR_INVALID_ERROR = 0,
    MOLD_ERROR_FAILED_TO_WRITE_BLOCK,
    MOLD_ERROR_FAILED_TO_RESET_ERROR_COUNTS,
    MOLD_ERROR_NULL_TERMINATE_AFTER_FIRST_WORD,
    MOLD_ERROR_TEST,

    // how many errors are there?
    MOLD_ERROR_NUM,
} MoldError;

void raise_error(MoldError error);

void reset_errors();

void list_errors();

uint8_t in_error_state();
