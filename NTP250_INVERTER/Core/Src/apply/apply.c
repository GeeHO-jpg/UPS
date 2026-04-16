/*
 * apply.c
 *
 *  Created on: Apr 7, 2026
 *      Author: _TTTXN
 */


#include "apply.h"

#include "../comm/uart_comm.h"
#include "../sensor/DC_sensor.h"
#include "main.h"
#include <string.h>
#include <stdint.h>




extern UART_HandleTypeDef huart1;

DC_SensorRawValue_t out;
static uint32_t last_tick = 0U;
uint8_t dc_raw[21];

static void dc_init(void)
{
    UART_ID_Register(UART_COMM_1, &huart1);
    PZEM003_DC_Sensor_Init(UART_COMM_1);
}



static void dc_run(void)
{
    uint32_t now = HAL_GetTick();

    PZEM003_DC_Sensor_Task();

    if ((now - last_tick) >= 500U)
    {
        last_tick = now;
        PZEM003_DC_Sensor_Request();
    }

    if (PZEM003_DC_Sensor_GetRaw(dc_raw, sizeof(dc_raw)) != 0U)
    {
        /* ใช้ข้อมูลดิบ dc_raw ตรงนี้ */
    	PZEM003_DC_ParseRawValue(dc_raw,&out);
    }
}

void APP_Init(void)
{
	dc_init();
}

void APP_Run(void)
{
	dc_run();
}
