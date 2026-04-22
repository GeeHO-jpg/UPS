/*
 * apply.c
 */
#include "apply.h"
#include "../comm/uart_comm.h"
#include "../sensor/NTS250P_INVERTER.h"
#include "main.h"

extern UART_HandleTypeDef huart2;

InverterQData_t inv;

void APP_Init(void)
{

    NTS250_init(UART_COMM_2,&huart2);
}

void APP_Run(void)
{

    NTS250_Task();

    if (NTS250_GetLatest(&inv))
    {
        // ใช้ inv ได้
    }
    else
    {
        // ยังไม่มีข้อมูล valid
    }
}
