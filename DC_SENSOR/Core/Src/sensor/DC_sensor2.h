///*
// * DC_sensor2.h
// *
// *  Created on: Apr 9, 2026
// *      Author: _TTTXN
// */
//
//#ifndef SRC_SENSOR_DC_SENSOR2_H_
//#define SRC_SENSOR_DC_SENSOR2_H_
//
//#include "stm32f1xx_hal.h"
//#include <stdint.h>
//
//#define PZEM_READ_CMD_LEN        8U
//#define PZEM_READ_RESP_LEN       21U
//
//#define PZEM_FUNC_READ_INPUT     0x04U
//#define PZEM_REG_START_H         0x00U
//#define PZEM_REG_START_L         0x00U
//#define PZEM_REG_COUNT_H         0x00U
//#define PZEM_REG_COUNT_L         0x08U
//
////uint16_t PZEM_CRC16(const uint8_t *data, uint16_t len);
//
//uint8_t PZEM_BuildReadMeasurementCmd(uint8_t slave_addr,
//                                     uint8_t *cmd_out,
//                                     uint16_t cmd_out_size);
//
//uint8_t PZEM_CheckFrameCRC(const uint8_t *frame, uint16_t len);
//
//HAL_StatusTypeDef PZEM_ReadMeasurementRaw(UART_HandleTypeDef *huart,
//                                          uint8_t slave_addr,
//                                          uint8_t *rx_out,
//                                          uint16_t rx_out_size,
//                                          uint32_t timeout_ms);
//
//
//
//#endif /* SRC_SENSOR_DC_SENSOR2_H_ */
