#pragma once

void spi_controller_init();
void spi_start();
void spi_end();
char spi_transceive_char(char data);
// the transceive_char function can do that same
// but this makes it clear that we're only receiving and don't care about what's being sent
char spi_receive_char();
