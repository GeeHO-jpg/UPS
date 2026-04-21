/*
 * PZEM004.c
 *
 *  Created on: Apr 21, 2026
 *      Author: _TTTXN
 */


#include "PZEM004.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../comm/comm.h"
#include "main.h"


#define REG_VOLTAGE     0x0000
#define REG_CURRENT_L   0x0001
#define REG_CURRENT_H   0X0002
#define REG_POWER_L     0x0003
#define REG_POWER_H     0x0004
#define REG_ENERGY_L    0x0005
#define REG_ENERGY_H    0x0006
#define REG_FREQUENCY   0x0007
#define REG_PF          0x0008
#define REG_ALARM       0x0009

#define CMD_RHR         0x03
#define CMD_RIR         0X04
#define CMD_WSR         0x06
#define CMD_CAL         0x41
#define CMD_REST        0x42


#define WREG_ALARM_THR   0x0001
#define WREG_ADDR        0x0002

#define UPDATE_TIME     200

#define RESPONSE_SIZE 32
#define READ_TIMEOUT 100

#define INVALID_ADDRESS 0x00

#define PZEM_DEFAULT_ADDR   0xF8
#define PZEM_BAUD_RATE      9600


#define REQ_AC 8U
#define RES_AC 25U


static Uart_Comm_id_t ac_id = UART_COMM_MAX;
static uint8_t _addr = 0;

//static uint32_t _lastRead;
static uint8_t rx_buf[RES_AC];
static uint8_t tx_buf[REQ_AC];

static PZEM_State_t pzem_state = PZEM_STATE_IDLE;
static uint32_t pzem_state_tick = 0U;
static uint32_t pzem_last_read_tick = 0U;

static PZEMValues latest_value;
static uint8_t latest_valid = 0U;
static uint8_t new_data_ready = 0U;
// static uint8_t rx_buf[DC_RESP_LEN];

static const uint16_t crcTable[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};



static uint16_t CRC16(const uint8_t *data, uint16_t len)
{
    uint8_t nTemp; // CRC table index
    uint16_t crc = 0xFFFF; // Default value

    while (len--)
    {
        nTemp = *data++ ^ crc;
        crc >>= 8;
        crc ^= crcTable[nTemp];
    }
    return crc;
}

static void setCRC(uint8_t *buf, uint16_t len){
    if(len <= 2) // Sanity check
        return;

    uint16_t crc = CRC16(buf, len - 2); // CRC of data

    // Write high and low byte to last two positions
    buf[len - 2] = crc & 0xFF; // Low byte first
    buf[len - 1] = (crc >> 8) & 0xFF; // High byte second
}

bool checkCRC(const uint8_t *buf, uint16_t len){
    if(len <= 2) // Sanity check
        return false;

    uint16_t crc = CRC16(buf, len - 2); // Compute CRC of data
    return ((uint16_t)buf[len-2]  | (uint16_t)buf[len-1] << 8) == crc;
}


static void sendCmd8(uint8_t cmd, uint16_t rAddr, uint16_t val)
{  



    tx_buf[0] = _addr;              // Set slave address
    tx_buf[1] = cmd;                     // Set command

    tx_buf[2] = (rAddr >> 8) & 0xFF;     // Set high byte of register address
    tx_buf[3] = (rAddr) & 0xFF;          // Set low byte =//=

    tx_buf[4] = (val >> 8) & 0xFF;       // Set high byte of register value
    tx_buf[5] = (val) & 0xFF;            // Set low byte =//=

    setCRC(tx_buf, REQ_AC);                   // Set CRC of frame

    UART_Comm_Transmit_IT(ac_id, tx_buf, REQ_AC);

}

static uint16_t scale_u32_to_u16(uint32_t raw, uint32_t divisor)
{
    uint32_t scaled;

    if (divisor == 0U)
    {
        return 0U;
    }

    scaled = raw / divisor;

    if (scaled > 65535UL)
    {
        return 65535U;
    }

    return (uint16_t)scaled;
}


uint8_t PZEM004_init(UART_HandleTypeDef *huart, Uart_Comm_id_t id, uint8_t addr)
{
    if (addr < 0x01 || addr > 0xF8)
    {
        addr = PZEM_DEFAULT_ADDR;
    }

    if (UART_ID_Register(id, huart) == 0U)
    {
        return 0U;
    }

    _addr = addr;
    ac_id = id;
    return 1U;
}

// bool PZEM004_GETValues(PZEMValues *out)
// {
//     uint32_t start_tick;
//     uint16_t low_word;
//     uint16_t high_word;
//     uint32_t raw_current_x1000;
//     uint32_t raw_power_x10;
//     uint32_t raw_energy_wh;

//     if (out == NULL)
//     {
//         return false;
//     }

//     /* ยังไม่ถึงเวลาอัปเดต */
//     if ((HAL_GetTick() - _lastRead) < UPDATE_TIME)
//     {
//         return false;
//     }

//     _lastRead = HAL_GetTick();

//     /* ถ้า UART ยังไม่ว่าง ไม่เริ่มรอบใหม่ */
//     if (UART_Comm_IsBusy(ac_id) != 0U)
//     {
//         return false;
//     }

//     UART_Comm_ClearFlags(ac_id);

//     /* ส่งคำสั่งอ่าน 10 registers เริ่มที่ 0x0000 */
//     sendCmd8(CMD_RIR, 0x0000U, 0x000AU);

//     /* รอ TX เสร็จ */
//     start_tick = HAL_GetTick();
//     while (UART_Comm_IsTxDone(ac_id) == 0U)
//     {
//         if (UART_Comm_HasError(ac_id) != 0U)
//         {
//             UART_Comm_Abort(ac_id);
//             return false;
//         }

//         if ((HAL_GetTick() - start_tick) > READ_TIMEOUT)
//         {
//             UART_Comm_Abort(ac_id);
//             return false;
//         }
//     }

//     UART_Comm_ClearFlags(ac_id);

//     /* เริ่มรับ response */
//     if (UART_Comm_Receive_IT(ac_id, rx_buf, RES_AC) == 0U)
//     {
//         UART_Comm_Abort(ac_id);
//         return false;
//     }

//     /* รอ RX เสร็จ */
//     start_tick = HAL_GetTick();
//     while (UART_Comm_IsRxDone(ac_id) == 0U)
//     {
//         if (UART_Comm_HasError(ac_id) != 0U)
//         {
//             UART_Comm_Abort(ac_id);
//             return false;
//         }

//         if ((HAL_GetTick() - start_tick) > READ_TIMEOUT)
//         {
//             UART_Comm_Abort(ac_id);
//             return false;
//         }
//     }

//     /* ตรวจ frame ขั้นพื้นฐาน */
//     if (rx_buf[0] != _addr)
//     {
//         return false;
//     }

//     if (rx_buf[1] != CMD_RIR)
//     {
//         return false;
//     }

//     /* 10 registers = 20 data bytes */
//     if (rx_buf[2] != 20U)
//     {
//         return false;
//     }

//     if (checkCRC(rx_buf, RES_AC) == false)
//     {
//         return false;
//     }

//     /* voltage: 16-bit */
//     out->voltage_x10 = ((uint16_t)rx_buf[3] << 8) |
//                         (uint16_t)rx_buf[4];

//     /* current: low word before high word */
//     low_word  = ((uint16_t)rx_buf[5] << 8) | (uint16_t)rx_buf[6];
//     high_word = ((uint16_t)rx_buf[7] << 8) | (uint16_t)rx_buf[8];
//     raw_current_x1000 = ((uint32_t)high_word << 16) | (uint32_t)low_word;

//     /* power: low word before high word */
//     low_word  = ((uint16_t)rx_buf[9] << 8) | (uint16_t)rx_buf[10];
//     high_word = ((uint16_t)rx_buf[11] << 8) | (uint16_t)rx_buf[12];
//     raw_power_x10 = ((uint32_t)high_word << 16) | (uint32_t)low_word;

//     /* energy: low word before high word */
//     low_word  = ((uint16_t)rx_buf[13] << 8) | (uint16_t)rx_buf[14];
//     high_word = ((uint16_t)rx_buf[15] << 8) | (uint16_t)rx_buf[16];
//     raw_energy_wh = ((uint32_t)high_word << 16) | (uint32_t)low_word;

//     /* scale ลงเพื่อเก็บใน uint16_t */
//     out->current_x10 = scale_u32_to_u16(raw_current_x1000, 100U); /* x1000 -> x10 */
//     out->power_w     = scale_u32_to_u16(raw_power_x10, 10U);      /* x10 -> x1 */
//     out->energy_wh   = scale_u32_to_u16(raw_energy_wh, 1U);       /* Wh -> Wh */

//     out->frequency_x10 = ((uint16_t)rx_buf[17] << 8) |
//                           (uint16_t)rx_buf[18];

//     out->pf_x100 = ((uint16_t)rx_buf[19] << 8) |
//                     (uint16_t)rx_buf[20];

//     out->alarms = ((uint16_t)rx_buf[21] << 8) |
//                    (uint16_t)rx_buf[22];

//     return true;
// }

void PZEM004_Task(void)
{
    switch (pzem_state)
    {
        case PZEM_STATE_IDLE:
            {
                if ((HAL_GetTick() - pzem_last_read_tick) < UPDATE_TIME)
                {
                    break;
                }

                if (UART_Comm_IsBusy(ac_id) != 0U)
                {
                    break;
                }

                UART_Comm_ClearFlags(ac_id);
                pzem_state = PZEM_STATE_SEND_REQ;
                break;
            }
            

        case PZEM_STATE_SEND_REQ:
        {
            sendCmd8(CMD_RIR, 0x0000U, 0x000AU);
            pzem_state_tick = HAL_GetTick();
            pzem_state = PZEM_STATE_WAIT_TX_DONE;
            break;
        }

            

        case PZEM_STATE_WAIT_TX_DONE:
        {
            if (UART_Comm_HasError(ac_id) != 0U)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if ((HAL_GetTick() - pzem_state_tick) > READ_TIMEOUT)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if (UART_Comm_IsTxDone(ac_id) != 0U)
            {
                UART_Comm_ClearFlags(ac_id);
                pzem_state = PZEM_STATE_START_RX;
            }
            break;
        }

        case PZEM_STATE_START_RX:
        {
            if (UART_Comm_Receive_IT(ac_id, rx_buf, RES_AC) == 0U)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            pzem_state_tick = HAL_GetTick();
            pzem_state = PZEM_STATE_WAIT_RX_DONE;
            break;
        }

        case PZEM_STATE_WAIT_RX_DONE:
        {
            if (UART_Comm_HasError(ac_id) != 0U)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if ((HAL_GetTick() - pzem_state_tick) > READ_TIMEOUT)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if (UART_Comm_IsRxDone(ac_id) != 0U)
            {
                pzem_state = PZEM_STATE_PARSE;
            }
            break;
        }

        case PZEM_STATE_PARSE:
        {
            uint16_t low_word;
            uint16_t high_word;
            uint32_t raw_current_x1000;
            uint32_t raw_power_x10;
            uint32_t raw_energy_wh;

            if (rx_buf[0] != _addr)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if (rx_buf[1] != CMD_RIR)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if (rx_buf[2] != 20U)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            if (checkCRC(rx_buf, RES_AC) == false)
            {
                pzem_state = PZEM_STATE_ERROR;
                break;
            }

            latest_value.voltage_x10 = ((uint16_t)rx_buf[3] << 8) | rx_buf[4];

            low_word  = ((uint16_t)rx_buf[5] << 8) | rx_buf[6];
            high_word = ((uint16_t)rx_buf[7] << 8) | rx_buf[8];
            raw_current_x1000 = ((uint32_t)high_word << 16) | low_word;

            low_word  = ((uint16_t)rx_buf[9] << 8) | rx_buf[10];
            high_word = ((uint16_t)rx_buf[11] << 8) | rx_buf[12];
            raw_power_x10 = ((uint32_t)high_word << 16) | low_word;

            low_word  = ((uint16_t)rx_buf[13] << 8) | rx_buf[14];
            high_word = ((uint16_t)rx_buf[15] << 8) | rx_buf[16];
            raw_energy_wh = ((uint32_t)high_word << 16) | low_word;

            latest_value.current_x10 = scale_u32_to_u16(raw_current_x1000, 100U);
            latest_value.power_w     = scale_u32_to_u16(raw_power_x10, 10U);
            latest_value.energy_wh   = scale_u32_to_u16(raw_energy_wh, 1U);

            latest_value.frequency_x10 = ((uint16_t)rx_buf[17] << 8) | rx_buf[18];
            latest_value.pf_x100       = ((uint16_t)rx_buf[19] << 8) | rx_buf[20];
            latest_value.alarms        = ((uint16_t)rx_buf[21] << 8) | rx_buf[22];

            latest_valid = 1U;
            new_data_ready = 1U;
            pzem_last_read_tick = HAL_GetTick();
            pzem_state = PZEM_STATE_IDLE;
            break;
        }

        case PZEM_STATE_ERROR:
        {
            UART_Comm_Abort(ac_id);
            UART_Comm_ClearFlags(ac_id);
            pzem_state = PZEM_STATE_IDLE;
            break;
        }

        default:
            pzem_state = PZEM_STATE_IDLE;
            break;
    }
}

uint8_t PZEM004_GetLatest(PZEMValues *out)
{
    if (out == NULL)
    {
        return 0U;
    }

    if (latest_valid == 0U)
    {
        return 0U;
    }

    *out = latest_value;
    return 1U;
}
