#pragma once

#include "flash_blocks.h"

void uart_print_uint8_t_dec(uint8_t val);
void uart_print_uint8_t_hex(uint16_t val);
void uart_print_uint16_t_hex(uint16_t val);
void uart_print_uint32_t_hex(uint32_t val);
void uart_print_uint64_t_hex(uint64_t val);
void uart_print_float(float val);
void uart_print_bool(uint8_t val);
// The *expected_timestamp is the last blocks timestamp plus the heartbeat interval.
// It will be updated if this is a timestampt block.
// If expected_timestamp is NULL it won't be updated and not displayed either.
void uart_print_flash_block(GenericFlashBlock* block, uint64_t* expected_timestamp);
void uart_print_kit_logo();
