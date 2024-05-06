#pragma once
#include <avr/sfr_defs.h>

// This implements a simple log-strucuted file system:
// Every time a new block is written, the free flag is unset.
// The location of the next free block can then found by looking through the flash
// and searching for the first free block (using a bianry search).
// Only the entire chip gets ever erased.
//
// The flash is separated into pages of 256 bytes.

// The flash storage is set to all 1 after being erased.
// Therefore the free flag is set for all free blocks.
#define FLASH_BLOCK_FLAG_FREE 0x80
// #define FLASH_BLOCK_FLAG_RESERVED = 0x40
// #define FLASH_BLOCK_FLAG_RESERVED = 0x20
// #define FLASH_BLOCK_FLAG_RESERVED = 0x10

// There are a maximum of 8 different block types.
#define FLASH_BLOCK_FLAG_TYPE 0x0f

#define FLASH_BLOCK_SIZE      8
#define FLASH_BLOCK_ADDR_MASK 0b111

#define FLASH_SIZE       0x1000000
#define FLASH_BLOCKS_NUM (FLASH_SIZE / FLASH_BLOCK_SIZE)

// values for FLASH_BLOCK_FLAG_TYPE in the flag byte of each block
typedef enum _flashBlockType {
    SENSOR_DATA = 0x0,
    TIMESTAMP   = 0x1,
} FlashBlockType;

// size of 8 bytes
// -> 32 blocks per page
// -> 2M blocks on a 128Mbit flash chip
typedef struct __attribute__((packed)) _genericFlashBlock {
    char padding[7];
    // see FLASH_BLOCK_FLAG_[...] definitions
    uint8_t flags;
} GenericFlashBlock;

// Every time the system receives a timing interrupt we log the sensor data to the flash.
// That way we keep track of the current time.
typedef struct __attribute__((packed)) _flashSensorData {
    uint16_t temperature;
    uint16_t humidity;
    uint8_t  temperature_crc;
    uint8_t  humidity_crc;

    char padding[1];
    // flags from the generic block definition
    uint8_t flags;
} FlashSensorData;

// The user can update the current time via the USB terminal.
// That creates an instance of this struct the offset to which is used to
// determine the time a sensor data block has been created.
typedef struct __attribute__((packed)) _flashTimestamp {
    // we need to fit this struct into 8 bytes and the first 8 bits
    // of the time will be zero for the foreseeable future anyways
    uint64_t unix_second_timestamp : 56;
    // flags from the generic block definition
    uint8_t flags;
} FlashTimestamp;
