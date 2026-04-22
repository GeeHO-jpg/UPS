/*
 * apply.c
 */
#include "apply.h"
#include "../comm/comm.h"
#include "../sensor/PZEM003.h"
#include "main.h"
#include <string.h>

extern UART_HandleTypeDef huart1;

//InverterQData_t inv;
PZEM003Values_t value;
void APP_Init(void)
{
	PZEM003_init(&huart1, UART_COMM_1, 0x01);
}

void APP_Run(void)
{
    PZEM003_Task();

    if (PZEM003_GetLatest(&value))
    {
        /* เอาค่า value ไปใช้ */
    }
    else
    {
        memset(&value, 0, sizeof(value)-2U);

    }

}
