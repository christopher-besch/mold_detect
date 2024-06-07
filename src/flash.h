#pragma once
#include "flash_blocks.h"

// driver for w25q128 flash

void flash_init();

void flash_write_next_block(GenericFlashBlock* block);

void flash_chip_erase();

void flash_print_usage();

void flash_print_all_blocks();
