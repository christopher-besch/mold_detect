#pragma once
#include "flash_structs.h"

// driver for w25q128 flash

void flash_init();

// erase a 4K bytes block
// the 12 least significant address bits need to be 0
// return 0 on success and -1 on failure
void flash_sector_erase(uint32_t address);

// write nbytes bytes from flash chip at address address to buf
// The flash chip may not be in power down mode
void flash_read_data(uint32_t address, void* buf, uint8_t nbytes);

// Write nbytes bytes from data to flash chip starting at address address.
// The area written to must be erased beforehand.
// The flash chip may not be in power down mode
// more than 256 bytes (one page) can't be written with one write command
//
// excempt from data sheet p. 37:
//
// "If an entire 256 byte page is to be programmed, the last address byte (the 8 least significant address bits)
// should be set to 0. If the last address byte is not zero, and the number of clocks exceeds the remaining
// page length, the addressing will wrap to the beginning of the page. In some cases, less than 256 bytes (a
// partial page) can be programmed without having any effect on other bytes within the same page. One
// condition to perform a partial page program is that the number of clocks cannot exceed the remaining
// page length. If more than 256 bytes are sent to the device the addressing will wrap to the beginning of the
// page and overwrite previously sent data."
void flash_write_data(uint32_t address, void* data, uint8_t nbytes);

void flash_find_next_free_block();
