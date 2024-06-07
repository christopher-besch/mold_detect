#include "i2c.h"
#include "flash.h"

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#define WRITE 0x00
#define READ  0x01

#define STATUS_MASK    0xf8
#define START          0x08
#define ADR_W_ACK      0x18
#define ADR_R_ACK      0x40
#define DATA_TRANS_ACK 0x28
#define DATA_REC_ACK   0x50

// connected ADDR to gnd
#define SHT30_ADDR            0x44
#define SHT30_ADDR_W          (SHT30_ADDR << 1 | WRITE)
#define SHT30_ADDR_R          (SHT30_ADDR << 1 | READ)
#define HIGH_REP_CLK_STRETCH1 0x2c
#define HIGH_REP_CLK_STRETCH0 0x06
// minimal waiting time
#define SCL_FREE 1

void await_transmission_conclusion()
{
    while(!(TWCR & (1 << TWINT)))
        ;
}
int check_status(uint8_t expected_status)
{
    return (TWSR & STATUS_MASK) == expected_status;
}

// send start condition
// return 0 on success
// return -1 on failure
int i2c_start()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    await_transmission_conclusion();
    if(!check_status(START))
        return -1;
    return 0;
}

// send stop condition
// return 0 on success
// return -1 on failure
int i2c_stop()
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    return 0;
}

// send data or address
// return 0 on success
// return -1 on failure
int i2c_send(uint8_t data, uint8_t expected_status)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    await_transmission_conclusion();
    if(!check_status(expected_status))
        return -1;
    return 0;
}

// receive data
// send ACK after receive
// return 0 on success
// return -1 on failure
int i2c_receive(uint8_t* data)
{
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
    await_transmission_conclusion();
    if(!check_status(DATA_REC_ACK))
        return -1;
    *data = TWDR;
    return 0;
}

int i2c_measure_temp_hum(FlashSensorData* sensor_data)
{
    // send measurement request //
    if(i2c_start())
        return -1;
    // send address in controller write mode
    if(i2c_send(SHT30_ADDR_W, ADR_W_ACK))
        return -1;
    // send command MSB
    if(i2c_send(HIGH_REP_CLK_STRETCH1, DATA_TRANS_ACK))
        return -1;
    // send command LSB
    if(i2c_send(HIGH_REP_CLK_STRETCH0, DATA_TRANS_ACK))
        return -1;
    if(i2c_stop())
        return -1;
    _delay_ms(SCL_FREE);

    // receive results //
    if(i2c_start())
        return -1;
    if(i2c_send(SHT30_ADDR_R, ADR_R_ACK))
        return -1;
    uint8_t temp1;
    uint8_t temp0;
    uint8_t temp_crc;
    uint8_t hum1;
    uint8_t hum0;
    uint8_t hum_crc;
    if(i2c_receive(&temp1))
        return -1;
    if(i2c_receive(&temp0))
        return -1;
    if(i2c_receive(&temp_crc))
        return -1;
    if(i2c_receive(&hum1))
        return -1;
    if(i2c_receive(&hum0))
        return -1;
    if(i2c_receive(&hum_crc))
        return -1;
    if(i2c_stop())
        return -1;

    // TODO: validate CRCs
    flash_create_sensor_data_block(sensor_data, (uint16_t)temp1 << 8 | (uint16_t)temp0, (uint16_t)hum1 << 8 | (uint16_t)hum0, temp_crc, hum_crc);
    return 0;
}
