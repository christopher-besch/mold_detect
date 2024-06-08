#pragma once

#include <avr/sfr_defs.h>

#define MAX_CMD_LENGTH (uint8_t)80

void          uart_init();
void          uart_trans(unsigned char data);
unsigned char uart_rec();
// The returned chars are valid until the next call of this function.
// The same piece of memory is being used
char* uart_rec_line();

void uart_print(const char* str);
void uart_println(const char* str);
