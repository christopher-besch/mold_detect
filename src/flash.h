#pragma once
#include "flash_blocks.h"

// driver for w25q128 flash
//
// The flash has a two power states:
// - power up
// - power down
//
// The flash starts in power up after flash_init() and
// gets put into power down mode when the system waits for the next timer interrupt
// to perform the next measurement.

void flash_init();

uint8_t flash_is_full();

void flash_write_next_block(GenericFlashBlock* block);

void flash_chip_erase();

void flash_print_usage();

void flash_print_all_blocks();

// print the timestamp of the last block
void flash_print_cur_timestamp();

void flash_power_down();
void flash_power_up();
