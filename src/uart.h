#pragma once

void uart_init();
void uart_trans(unsigned char data);
void uart_print(const char* str);
void uart_println(const char* str);
