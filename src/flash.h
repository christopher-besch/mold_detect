#pragma once
#include <avr/sfr_defs.h>

// This implements a simple log-strucuted file system:
// Every time a new block is written, this invalid is unset.
// The location of the next free block can then found by looking through the flash
// and searching for the first invalid block (using a bianry search).
// Only the entire chip gets ever erased.
//
// The flash is separated into pages of 256 bytes.

// The flash storage is set to all 1 after being erased.
// Therefore the invalid flag is set for all free blocks.
#define FLASH_BLOCK_FLAG_INVALID = 0x80
// #define FLASH_BLOCK_FLAG_RESERVED = 0x40
// #define FLASH_BLOCK_FLAG_RESERVED = 0x20
// #define FLASH_BLOCK_FLAG_RESERVED = 0x10

// There are a maximum of 8 different block types.
#define FLASH_BLOCK_FLAG_TYPE = 0x0f

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

void flash_init();

// erase a 4K bytes block
// the 12 least significant address bits need to be 0
// return 0 on success and -1 on failure
int flash_sector_erase(uint32_t address);

// write nbytes bytes from flash chip at address address to buf
// The flash chip may not be in power down mode
//
// return 0 on success
// return -1 on failure
int flash_read_data(uint32_t address, void* buf, uint8_t nbytes);

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
int flash_write_data(uint32_t address, void* data, uint8_t nbytes);
