/*
 * apply_INVERTER.c
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */


#include "apply_INVERTER.h"

#include <stdint.h>
#include <main.h>


#include "../comm/uart_comm.h"

InverterQData_t inv_data;


extern UART_HandleTypeDef huart2;

static uint32_t last_tick = 0U;

uint8_t NTS250_APP_init()
{
    if(!UART_ID_Register(UART_COMM_2, &huart2))
    {
        return 0;
    }
    NTS250_init(UART_COMM_2);
    return 1;
}



uint8_t NTS250_APP_run(InverterQData_t *out)
{
    uint32_t now = HAL_GetTick();

    if (out == NULL)
    {
        return 0U;
    }

    NTS250_Task();

    if ((now - last_tick) >= 800U)
    {
        last_tick = now;
        (void)NTS250_Q_CMD_Request();
    }

    if (NTS250_GetQData(out) != 0U)
    {
        return 1U;
    }

    return 0U;
}



