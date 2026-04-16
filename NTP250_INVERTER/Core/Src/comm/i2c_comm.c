#include "main.h"
#include "i2c_comm.h"

#include <string.h>
#include <stdint.h>

#if USE_I2C == 1

#include "../apply/ring_buffer.h"

extern I2C_HandleTypeDef hi2c1;

/* external ring buffers */
extern RingBuffer i2c_tx_slave_rb;
extern RingBuffer i2c_rx_slave_rb;
extern RingBuffer i2c_tx_master_rb;
extern RingBuffer i2c_rx_master_rb;

#define slaveAddress (0x01 << 1)

static uint8_t buffer_for_send[MAX_BUF];
static uint8_t buffer_for_receive[MAX_BUF];

volatile uint16_t slave_tx_complete = 0U;
volatile uint16_t slave_rx_complete = 0U;
volatile uint16_t slave_error_count = 0U;
volatile uint8_t slave_tx_busy = 0U;
volatile uint8_t slave_rx_busy = 0U;
volatile uint8_t get_from_master_flag = 0U;

volatile uint8_t slave_need_relisten = 0U;

static void slave_prepare_tx_buffer(void)
{
    uint16_t tx_len;

    memset(buffer_for_send, 0, sizeof(buffer_for_send));

    tx_len = rb_get_count(&i2c_tx_slave_rb);
    if (tx_len > sizeof(buffer_for_send))
    {
        tx_len = sizeof(buffer_for_send);
    }

    if (tx_len > 0U)
    {
        (void)rb_get(&i2c_tx_slave_rb, buffer_for_send, tx_len);
    }
}

//void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c,
//                          uint8_t TransferDirection,
//                          uint16_t AddrMatchCode)
//{
//    (void)AddrMatchCode;
//
//    if (hi2c != &hi2c1)
//    {
//        return;
//    }
//
//    if (TransferDirection == I2C_DIRECTION_TRANSMIT)
//    {
//        /* master -> slave */
//        slave_rx_busy = 1U;
//        HAL_I2C_Slave_Sequential_Receive_IT(hi2c,buffer_for_receive,sizeof(buffer_for_receive), I2C_FIRST_AND_LAST_FRAME);
//    }
//    else if (TransferDirection == I2C_DIRECTION_RECEIVE)
//    {
//        slave_prepare_tx_buffer();
//
//        slave_tx_busy = 1U;
//        HAL_I2C_Slave_Sequential_Transmit_IT(hi2c,buffer_for_send,sizeof(buffer_for_send),I2C_FIRST_AND_LAST_FRAME);
//    }
//}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c,
                          uint8_t TransferDirection,
                          uint16_t AddrMatchCode)
{
    HAL_StatusTypeDef st;
    (void)AddrMatchCode;

    if (hi2c != &hi2c1)
    {
        return;
    }

    if (TransferDirection == I2C_DIRECTION_TRANSMIT)
    {
        slave_rx_busy = 0U;

        st = HAL_I2C_Slave_Sequential_Receive_IT(
                hi2c,
                buffer_for_receive,
                sizeof(buffer_for_receive),
                I2C_FIRST_AND_LAST_FRAME);

        if (st == HAL_OK)
        {
            slave_rx_busy = 1U;
        }
        else
        {
            slave_error_count++;
            slave_need_relisten = 1U;
        }
    }
    else if (TransferDirection == I2C_DIRECTION_RECEIVE)
    {
        slave_prepare_tx_buffer();
        slave_tx_busy = 0U;

        st = HAL_I2C_Slave_Sequential_Transmit_IT(
                hi2c,
                buffer_for_send,
                sizeof(buffer_for_send),
                I2C_FIRST_AND_LAST_FRAME);

        if (st == HAL_OK)
        {
            slave_tx_busy = 1U;
        }
        else
        {
            slave_error_count++;
            slave_need_relisten = 1U;
        }
    }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
    {
        slave_tx_busy = 0U;
        slave_tx_complete++;
    }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
    {
        slave_rx_busy = 0U;
        slave_rx_complete++;

        /* ตอนนี้ใช้ fixed 64 ไปก่อน */
        (void)rb_put_exact(&i2c_rx_slave_rb,buffer_for_receive,sizeof(buffer_for_receive));

        get_from_master_flag = 1U;
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
    {
        slave_tx_busy = 0U;
        slave_rx_busy = 0U;
        slave_error_count++;
        slave_need_relisten = 1U;
    }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1)
    {
        if (HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK)
        {
            slave_error_count++;
            slave_need_relisten = 1U;
        }
    }
}

#endif
