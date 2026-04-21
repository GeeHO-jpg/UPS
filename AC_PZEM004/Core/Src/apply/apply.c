/*
 * apply.c
 */
#include "apply.h"
#include "../comm/comm.h"
#include "../sensor/PZEM004.h"
#include "main.h"

extern UART_HandleTypeDef huart1;

//InverterQData_t inv;
PZEMValues value;
void APP_Init(void)
{
	PZEM004_init(&huart1, UART_COMM_1, 0x01);
}

void APP_Run(void)
{
    PZEM004_Task();

    if (PZEM004_GetLatest(&value))
    {
        /* เอาค่า value ไปใช้ */
    }
}
