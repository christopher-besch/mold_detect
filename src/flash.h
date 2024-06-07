#pragma once
#include "flash_structs.h"

// driver for w25q128 flash

void flash_init();

void flash_write_next_block(GenericFlashBlock* block);

void flash_chip_erase();

void flash_print_usage();

void flash_print_all_blocks();

// only create the block, don't save it to the flash just yet
void create_flash_sensor_data(FlashSensorData* sensor_data_block, uint32_t temp, uint32_t hum, uint32_t temp_crc, uint32_t hum_crc);
void create_flash_timestamp(FlashTimestamp* timestamp_block, uint64_t unix_second_timestamp);
