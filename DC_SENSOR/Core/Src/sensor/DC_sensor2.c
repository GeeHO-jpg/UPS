///*
// * DC_sensor2.c
// *
// *  Created on: Apr 9, 2026
// *      Author: _TTTXN
// */
//
//
//#include "DC_sensor2.h"
//#include <string.h>
//
//static uint16_t PZEM_CRC16(const uint8_t *data, uint16_t len)
//{
//    uint16_t crc = 0xFFFFU;
//    uint16_t i;
//    uint8_t j;
//
//    if (data == NULL)
//    {
//        return 0U;
//    }
//
//    for (i = 0U; i < len; i++)
//    {
//        crc ^= data[i];
//
//        for (j = 0U; j < 8U; j++)
//        {
//            if ((crc & 0x0001U) != 0U)
//            {
//                crc = (uint16_t)((crc >> 1U) ^ 0xA001U);
//            }
//            else
//            {
//                crc >>= 1U;
//            }
//        }
//    }
//
//    return crc;
//}
//
//uint8_t PZEM_BuildReadMeasurementCmd(uint8_t slave_addr,
//                                     uint8_t *cmd_out,
//                                     uint16_t cmd_out_size)
//{
//    uint16_t crc;
//
//    if ((cmd_out == NULL) || (cmd_out_size < PZEM_READ_CMD_LEN))
//    {
//        return 0U;
//    }
//
//    /* command: addr + 0x04 + 0x0000 + 0x0008 + crc */
//    cmd_out[0] = slave_addr;
//    cmd_out[1] = PZEM_FUNC_READ_INPUT;
//    cmd_out[2] = PZEM_REG_START_H;
//    cmd_out[3] = PZEM_REG_START_L;
//    cmd_out[4] = PZEM_REG_COUNT_H;
//    cmd_out[5] = PZEM_REG_COUNT_L;
//
//    crc = PZEM_CRC16(cmd_out, 6U);
//
//    /* Modbus RTU ใช้ CRC low byte ก่อน */
//    cmd_out[6] = (uint8_t)(crc & 0xFFU);
//    cmd_out[7] = (uint8_t)((crc >> 8U) & 0xFFU);
//
//    return 1U;
//}
//
//uint8_t PZEM_CheckFrameCRC(const uint8_t *frame, uint16_t len)
//{
//    uint16_t crc_calc;
//    uint16_t crc_recv;
//
//    if ((frame == NULL) || (len < 4U))
//    {
//        return 0U;
//    }
//
//    crc_calc = PZEM_CRC16(frame, (uint16_t)(len - 2U));
//    crc_recv = (uint16_t)frame[len - 2U] |
//               (uint16_t)((uint16_t)frame[len - 1U] << 8U);
//
//    return (crc_calc == crc_recv) ? 1U : 0U;
//}
//
//HAL_StatusTypeDef PZEM_ReadMeasurementRaw(UART_HandleTypeDef *huart,
//                                          uint8_t slave_addr,
//                                          uint8_t *rx_out,
//                                          uint16_t rx_out_size,
//                                          uint32_t timeout_ms)
//{
//    uint8_t tx_cmd[PZEM_READ_CMD_LEN];
//
//    if ((huart == NULL) || (rx_out == NULL) || (rx_out_size < PZEM_READ_RESP_LEN))
//    {
//        return HAL_ERROR;
//    }
//
//    if (PZEM_BuildReadMeasurementCmd(slave_addr, tx_cmd, sizeof(tx_cmd)) == 0U)
//    {
//        return HAL_ERROR;
//    }
//
//    memset(rx_out, 0, rx_out_size);
//
//    /*
//     * ถ้าใช้ MAX485 / RS485 transceiver ที่ต้องคุม DE/RE เอง
//     * ให้เปิดโหมดส่งก่อนตรงนี้
//     * เช่น:
//     * HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);
//     */
//
//    if (HAL_UART_Transmit(huart, tx_cmd, PZEM_READ_CMD_LEN, timeout_ms) != HAL_OK)
//    {
//        return HAL_ERROR;
//    }
//
//    /*
//     * ถ้าคุม DE/RE เอง ให้สลับกลับเป็นโหมดรับหลังส่งเสร็จ
//     * เช่น:
//     * HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);
//     */
//
//    /* เวอร์ชันง่าย: รับ correct reply แบบ fix 21 bytes */
//    if (HAL_UART_Receive(huart, rx_out, PZEM_READ_RESP_LEN, 1000) != HAL_OK)
//    {
//        return HAL_TIMEOUT;
//    }
//
//    /* เช็ค header แบบพื้นฐาน */
//    if (rx_out[0] != slave_addr)
//    {
//        return HAL_ERROR;
//    }
//
//    if (rx_out[1] != PZEM_FUNC_READ_INPUT)
//    {
//        return HAL_ERROR;
//    }
//
//    if (rx_out[2] != 0x10U)   /* 8 registers = 16 data bytes */
//    {
//        return HAL_ERROR;
//    }
//
//    /* เช็ค CRC */
//    if (PZEM_CheckFrameCRC(rx_out, PZEM_READ_RESP_LEN) == 0U)
//    {
//        return HAL_ERROR;
//    }
//
//    return HAL_OK;
//}
