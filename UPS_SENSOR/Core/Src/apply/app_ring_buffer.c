/*
 * app_ring_buffer.c
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#include "app_ring_buffer.h"
#include <stdint.h>
#include <main.h>



#define RB_size 512

RingBuffer i2c_tx_slave_rb;
RingBuffer i2c_rx_slave_rb;
RingBuffer i2c_tx_master_rb;
RingBuffer i2c_rx_master_rb;


uint8_t app_slave_rb_init(void)
{
    if (rb_init(&i2c_tx_slave_rb, RB_size) != 0)
    {
        return 0U;
    }

    if (rb_init(&i2c_rx_slave_rb, RB_size) != 0)
    {
        return 0U;
    }
    InitializeI2CComm(I2CSlaveReadByte, I2CSlaveWriteByte);

    return 1U;
}
