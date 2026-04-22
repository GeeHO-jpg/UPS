/*
 * NTS250P_INVERTER.h
 *
 *  Created on: Apr 22, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_SENSOR_NTS250P_INVERTER_H_
#define SRC_SENSOR_NTS250P_INVERTER_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "../comm/comm.h"

#include <stdint.h>
#include "../comm/uart_comm.h"

typedef struct
{
    uint8_t inverter_mode;
    uint8_t bypass_mode;
    uint8_t utility_present;
    uint8_t utility_charger_enable;
    uint8_t solar_charger_enable;
    uint8_t saving_mode;
    uint8_t battery_exhausted;
    uint8_t shutdown_mode;
    uint8_t battery_ovp;
    uint8_t remote_shutdown;
    uint8_t olp_100_115;
    uint8_t olp_115_150;
    uint8_t olp_over_150;
    uint8_t otp;
    uint8_t inv_uvp;
    uint8_t inv_ovp;
    uint8_t inv_fault;
    uint8_t eeprom_error;
    uint8_t system_shutdown;
} QStatusBits_t;

typedef struct
{
    uint16_t output_voltage_ac;
    uint8_t  load_percent_digital;
    uint16_t battery_voltage_x10;     // เช่น 24.5V -> 245
    uint8_t  battery_percent;
    int16_t  heatsink_temp_x10;       // เช่น 32.1C -> 321
    uint16_t utility_voltage;
    uint16_t output_frequency_x10;    // เช่น 50.0Hz -> 500
    uint16_t dc_bus_voltage;
    uint8_t  load_percent_analog;
    QStatusBits_t status;
} InverterQData_t;

void NTS250_init(Uart_Comm_id_t uart_id,UART_HandleTypeDef *huart);
void NTS250_Task(void);
uint8_t NTS250_GetLatest(InverterQData_t *out);


#endif /* SRC_SENSOR_NTS250P_INVERTER_H_ */
