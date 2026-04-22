/*
 * apply_DC_sensor.c
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#include "apply_DC_sensor.h"

#include <stdint.h>
#include <main.h>
#include <string.h>

#include "../comm/adc_comm.h"


extern UART_HandleTypeDef huart1;


PZEM003Values_t DCvalue;
uint8_t DC_app_init(void)
{
	return PZEM003_init(&huart1, UART_COMM_1, 0x01);
}

uint8_t DC_app_Run(PZEM003Values_t *out)
{
    if (out == NULL)
    {
        return 0U;
    }

    PZEM003_Task();

    if (PZEM003_GetLatest(&DCvalue))
    {
        /* เอาค่า value ไปใช้ */
        *out = DCvalue;
        return 1U;
    }
    else
    {
        memset(out, 0, sizeof(DCvalue)-2);
        return 0U;
    }
}


//
//extern UART_HandleTypeDef huart1;
//
//DC_SensorRawValue_t out;
//static uint32_t last_tick = 0U;
//uint8_t dc_raw[21];
//
//uint8_t dc_app_init()
//{
//
//    PZEM003_DC_Sensor_Init(UART_COMM_1,&huart1);
//    return 1;
//}
//
//
//
//uint8_t dc_app_run(DC_SensorRawValue_t *out)
//{
//    if (out == NULL)
//    {
//        return 0U;
//    }
//
//    uint32_t now = HAL_GetTick();
//
//    PZEM003_DC_Sensor_Task();
//
//    if ((now - last_tick) >= 500U)
//    {
//        last_tick = now;
//        PZEM003_DC_Sensor_Request();
//    }
//
//    if (PZEM003_DC_Sensor_GetRaw(dc_raw, sizeof(dc_raw)) != 0U)
//    {
//        PZEM003_DC_ParseRawValue(dc_raw, out);
//        return 1U;
//    }
//
//    return 0U;
//}


