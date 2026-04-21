#include "main.h"
#include "i2c_comm.h"
#include <string.h>
#include <stdint.h>
#include "ring_buffer.h"

extern I2C_HandleTypeDef hi2c2;

/* external ring buffers */
extern RingBuffer i2c_tx_slave_rb;
extern RingBuffer i2c_rx_slave_rb;
extern RingBuffer i2c_tx_master_rb;
extern RingBuffer i2c_rx_master_rb;

// #define slaveAddress (0x02 << 1)

static uint8_t buffer_for_send[64];
static uint8_t buffer_for_receive[64];
//static uint16_t tx_len = 0U;


volatile uint16_t master_tx_complete = 0U;
volatile uint16_t master_rx_complete = 0U;
volatile uint16_t master_rx_complete_flag = 0U;

volatile uint16_t master_error_count = 0U;
volatile uint8_t master_tx_busy = 0U;
volatile uint8_t master_rx_busy = 0U;

volatile uint8_t add_slave = 0;

void set_slave_address(uint8_t addr){
    add_slave = addr<<1;
}

void i2ccom_read_from_slave(void)
{
    if (master_rx_busy != 0U)
    {
        return;
    }

     memset(buffer_for_receive, 0, sizeof(buffer_for_receive));

    if (HAL_I2C_Master_Receive_IT(&hi2c2, (uint16_t)add_slave, buffer_for_receive, sizeof(buffer_for_receive)) == HAL_OK)
    {
        master_rx_busy = 1U;
    }
    else
    {
        master_error_count++;
    }
}

//uint8_t i2ccom_run(void)
//{
//    uint16_t used_len;
//
//    if (master_tx_busy != 0U)
//    {
//        return 0U;
//    }
//
//    used_len = rb_get_count(&i2c_tx_master_rb);
//    if (used_len == 0U)
//    {
//        return 0U;
//    }
//
//    if (used_len > sizeof(buffer_for_send))
//    {
//        used_len = sizeof(buffer_for_send);
//    }
//
//    memset(buffer_for_send, 0, sizeof(buffer_for_send));
//
//    used_len = rb_get(&i2c_tx_master_rb, buffer_for_send, used_len);
//    if (used_len == 0U)
//    {
//        return 0U;
//    }
//
//    if (HAL_I2C_Master_Transmit_IT(&hi2c1, (uint16_t)add_slave, buffer_for_send, sizeof(buffer_for_send)) == HAL_OK)
//    {
//        master_tx_busy = 1U;
//        return 1U;
//    }
//    else
//    {
//        master_error_count++;
//        return 0U;
//    }
//}


uint8_t i2ccom_run(void)
{
    uint16_t used_len = 0U;

    if (master_tx_busy != 0U)
    {
        return 0U;
    }

    used_len = rb_get_count(&i2c_tx_master_rb);
    if (used_len == 0U)
    {
        return 0U;
    }

    if (used_len > sizeof(buffer_for_send))
    {
        used_len = sizeof(buffer_for_send);
    }

    used_len = rb_get(&i2c_tx_master_rb, buffer_for_send, used_len);
    if (used_len == 0U)
    {
        return 0U;
    }

    if (HAL_I2C_Master_Transmit_IT(&hi2c2,add_slave,buffer_for_send,sizeof(buffer_for_send)) == HAL_OK)
    {
        return 1U;
    }

    return 0U;
}
//void i2ccom_run(void)
//{
//    uint16_t used_len;
//
//    if (master_tx_busy != 0U)
//    {
//        return;
//    }
//
//    used_len = rb_get_count(&i2c_tx_master_rb);
//    if (used_len == 0U)
//    {
//        return;
//    }
//
//    if (used_len > sizeof(buffer_for_send))
//    {
//        used_len = sizeof(buffer_for_send);
//    }
//
//    memset(buffer_for_send, 0, sizeof(buffer_for_send));
//
//    used_len = rb_get(&i2c_tx_master_rb, buffer_for_send, used_len);
//    if (used_len == 0U)
//    {
//        return;
//    }
//
//    /* ส่ง 64 byte ตลอด */
//    if (HAL_I2C_Master_Transmit_IT(&hi2c1,slaveAddress,buffer_for_send,sizeof(buffer_for_send)) == HAL_OK)
//    {
//        master_tx_busy = 1U;
//    }
//    else
//    {
//        master_error_count++;
//    }
//}

/*==================================================
 * CALLBACK
 *==================================================*/

//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//    if (hi2c == &hi2c1)
//    {
//        master_tx_busy = 0U;
//        master_tx_complete++;
//        HAL_I2C_Master_Receive_IT(&hi2c1, slaveAddress, buffer_for_receive, sizeof(buffer_for_receive));
//    }
//}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2)
    {
        master_rx_busy = 0U;
        master_rx_complete++;
        master_rx_complete_flag++;

        (void)rb_put_exact(&i2c_rx_master_rb, buffer_for_receive, sizeof(buffer_for_receive));
    }
}



void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2)
    {
        master_tx_busy = 0U;
        master_tx_complete++;


        if (HAL_I2C_Master_Receive_IT(&hi2c2, (uint16_t)add_slave,buffer_for_receive,sizeof(buffer_for_receive)) == HAL_OK)
        {
            master_rx_busy = 1U;
        }
        else
        {
            master_error_count++;
        }
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c2)
    {
        master_tx_busy = 0U;
        master_rx_busy = 0U;
        master_error_count++;
    }
}


