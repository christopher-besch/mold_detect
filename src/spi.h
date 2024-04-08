#pragma once

void spi_controller_init();
void spi_start();
void spi_end();
char spi_transceive_char(char data);
char spi_receive_char();
