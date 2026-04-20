/*
 * apply.c
 */
#include "apply_INVERTER.h"
#include "../comm/uart_comm.h"
#include <string.h>
#include "../sensor/NTS250_INVERTER.h"
#include "main.h"

extern UART_HandleTypeDef huart2;

InverterQData_t inv;

uint8_t NTS250_APP_init(void)
{
    if(!UART_ID_Register(UART_COMM_2, &huart2))
    {
    	return 0U;
    }
    NTS250_init(UART_COMM_2);
    return 1U;
}

uint8_t NTS250_APP_run(InverterQData_t *out)
{

    if (out == NULL)
    {
        return 0U;
    }

    NTS250_Task();

    if (NTS250_GetLatest(&inv))
    {
        *out = inv;
        return 1U;
    }
    else
    {
        memset(out, 0, sizeof(*out));
        return 0U;
    }
}
