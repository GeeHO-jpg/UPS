/*
 * apply_DC_sensor.c
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#include "apply_DC_sensor.h"

#include <stdint.h>
#include <main.h>


#include "../comm/uart_comm.h"


extern UART_HandleTypeDef huart1;

DC_SensorRawValue_t out;
static uint32_t last_tick = 0U;
uint8_t dc_raw[21];

uint8_t dc_app_init()
{
    if(!UART_ID_Register(UART_COMM_1, &huart1))
    {
        return 0;
    }
    PZEM003_DC_Sensor_Init(UART_COMM_1);
    return 1;
}



uint8_t dc_app_run(DC_SensorRawValue_t *out)
{
    if (out == NULL)
    {
        return 0U;
    }

    uint32_t now = HAL_GetTick();

    PZEM003_DC_Sensor_Task();

    if ((now - last_tick) >= 500U)
    {
        last_tick = now;
        PZEM003_DC_Sensor_Request();
    }

    if (PZEM003_DC_Sensor_GetRaw(dc_raw, sizeof(dc_raw)) != 0U)
    {
        PZEM003_DC_ParseRawValue(dc_raw, out);
        return 1U;
    }

    return 0U;
}


