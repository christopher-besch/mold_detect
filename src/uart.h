#pragma once

#include <stdint.h>

void uart_init();
void uart_trans(unsigned char data);
void uart_print(const char* str);
void uart_println(const char* str);
void uart_print_uint8_t(uint8_t val);
