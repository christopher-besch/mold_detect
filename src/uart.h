#pragma once

#include <stdint.h>

#define MAX_CMD_LENGTH (uint8_t)120

void          uart_init();
void          uart_trans(unsigned char data);
unsigned char uart_rec();

void uart_print(const char* str);
void uart_println(const char* str);
void uart_print_uint8_t(uint8_t val);

// The returned chars are valid until the next call of this function.
// The same piece of memory is being used
char* uart_rec_line();
