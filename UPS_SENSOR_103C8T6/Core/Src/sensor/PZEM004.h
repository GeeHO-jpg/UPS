/*
 * PZEM004.h
 *
 *  Created on: Apr 21, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_SENSOR_PZEM004_H_
#define SRC_SENSOR_PZEM004_H_

#include <stdint.h>
#include <stdbool.h>

#include "../comm/comm.h"

typedef enum
{
    PZEM004_STATE_IDLE = 0,
    PZEM004_STATE_SEND_REQ,
    PZEM004_STATE_WAIT_TX_DONE,
    PZEM004_STATE_START_RX,
    PZEM004_STATE_WAIT_RX_DONE,
    PZEM004_STATE_PARSE,
    PZEM004_STATE_ERROR
} PZEM_State_t;

typedef struct {
    uint16_t voltage_x10;
    uint16_t current_x10;
    uint16_t power_w;
    uint16_t energy_wh;
    uint16_t frequency_x10;
    uint16_t pf_x100;
    uint16_t alarms;
} PZEMValues;

uint8_t PZEM004_init(UART_HandleTypeDef *huart,Uart_Comm_id_t id,uint8_t addr);
// bool PZEM004_GETValues(PZEMValues *out);
void PZEM004_Task(void);
uint8_t PZEM004_GetLatest(PZEMValues *out);

#endif /* SRC_SENSOR_PZEM004_H_ */
