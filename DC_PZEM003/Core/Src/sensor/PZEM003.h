/*
 * PZEM003.h
 *
 *  Created on: Apr 22, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_SENSOR_PZEM003_H_
#define SRC_SENSOR_PZEM003_H_

#include <stdint.h>
#include <stdbool.h>

#include "../comm/comm.h"

typedef enum
{
    PZEM_STATE_IDLE = 0,
    PZEM_STATE_SEND_REQ,
    PZEM_STATE_WAIT_TX_DONE,
    PZEM_STATE_START_RX,
    PZEM_STATE_WAIT_RX_DONE,
    PZEM_STATE_PARSE,
    PZEM_STATE_ERROR
} PZEM003_State_t;

typedef struct {
    uint16_t voltage_x100;   // 100.00V => 10000
    uint16_t current_x100;   // 1.00A   => 100
    uint32_t power_x10;      // 100.0W  => 1000
    uint32_t energy_wh;      // 0Wh     => 0 
    uint16_t high_alarm_status;
    uint16_t low_alarm_status;
} PZEM003Values_t;

uint8_t PZEM003_init(UART_HandleTypeDef *huart,Uart_Comm_id_t id,uint8_t addr);
// bool PZEM004_GETValues(PZEMValues *out);
void PZEM003_Task(void);
uint8_t PZEM003_GetLatest(PZEM003Values_t *out);

#endif /* SRC_SENSOR_PZEM003_H_ */
