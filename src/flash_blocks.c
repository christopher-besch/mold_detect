#include "flash_blocks.h"

#include "error.h"
#include "measure.h"

FlashBlockType flash_get_block_type(uint8_t flags)
{
    FlashBlockType type = flags & FLASH_BLOCK_FLAG_TYPE;

    switch(type) {
    case SENSOR_DATA_BLOCK:
        break;
    case TIMESTAMP_BLOCK:
        break;
    default:
        raise_fatal_error(MOLD_ERROR_FLASH_GET_BLOCK_TYPE_INVALID_TYPE);
    }

    return type;
}
uint8_t flash_is_block_free(uint8_t flags)
{
    return flags & FLASH_BLOCK_FLAG_FREE;
}
uint8_t flash_is_block_atmos_bad(uint8_t flags)
{
    return flags & FLASH_BLOCK_FLAG_ATMOS_BAD;
}
uint8_t flash_is_block_err_set(uint8_t flags)
{
    return flags & FLASH_BLOCK_FLAG_ERR_SET;
}

void flash_set_block_flags(uint8_t* flags, FlashBlockType block_type)
{
    MD_ASSERT(flags, MOLD_ERROR_FLASH_SET_BLOCK_FLAGS_NULL);
    *flags = 0;
    *flags |= 0 * FLASH_BLOCK_FLAG_FREE;
    *flags |= measure_is_atmosphere_bad() * FLASH_BLOCK_FLAG_ATMOS_BAD;
    *flags |= in_error_state() * FLASH_BLOCK_FLAG_ERR_SET;
    *flags |= block_type;
}

void flash_create_sensor_data_block(FlashSensorData* sensor_data_block, uint32_t temp, uint32_t hum, uint32_t temp_crc, uint32_t hum_crc)
{
    MD_ASSERT(sensor_data_block, MOLD_ERROR_FLASH_CREATE_SENSOR_DATA_BLOCK_NULL);
    sensor_data_block->temperature     = temp;
    sensor_data_block->humidity        = hum;
    sensor_data_block->temperature_crc = temp_crc;
    sensor_data_block->humidity_crc    = hum_crc;
    sensor_data_block->padding[0]      = 0;
    flash_set_block_flags(&sensor_data_block->flags, SENSOR_DATA_BLOCK);
}

void flash_create_timestamp_block(FlashTimestamp* timestamp_block, uint64_t unix_second_timestamp)
{
    MD_ASSERT(timestamp_block, MOLD_ERROR_FLASH_CREATE_TIMESTAMP_BLOCK_NULL);
    timestamp_block->unix_second_timestamp = unix_second_timestamp;
    flash_set_block_flags(&timestamp_block->flags, TIMESTAMP_BLOCK);
}
