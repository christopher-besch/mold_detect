#pragma once
#include "flash_structs.h"

// driver for w25q128 flash

void flash_init();

void flash_write_next_block(GenericFlashBlock* block);

void flash_chip_erase();

void flash_print_usage();
