#pragma once

#define MOLD_ERROR_ (1 << 0)
// #define MOLD_ERROR_RESERVED (1 << 1)
// ...
// #define MOLD_ERROR_RESERVED (1 << 63)

// This is the very first sector (4k bytes = 16pages) in the flash
// containing all kinds of rarely written data.
// As the Atmega328p only has 2k bytes of RAM
typedef struct __attribute__((packed)) _flashHeader {
    // how often did the whole chip get erased?
    // parts of the chip never get erased
    // As the flash chip is rated for 100k erase cycles
    // 32bit should be enough.
    uint64_t erase_number;
    uint64_t error_flag;
    char     comment[16 + 16];
} FlashHeader;
