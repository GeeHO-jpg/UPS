/*
 * ADC_APP.c
 *
 *  Created on: Apr 6, 2026
 *      Author: _TTTXN
 */


#include "ADC_APP.h"
#include "main.h"
#include "../comm/adc_comm.h"
#include "../sensor/AC_sensor.h"

#define FREQUENCY 50
#define SENSITIVITY 1000.0f
extern ADC_HandleTypeDef hadc1;

volatile int16_t rawdata = 0;
volatile int16_t offset = 0;
volatile uint8_t AC_VOLTAGE = 0;
static void comm_init()
{
	ADC_ID_Register(ADC_COMM_1,&hadc1);
	ADC_Comm_Start(ADC_COMM_1);
}

void app_init()
{
	comm_init();
	AC_init(FREQUENCY,SENSITIVITY);
}

void app_run()
{
	if(ADC_Comm_GetLatest(ADC_COMM_1,&rawdata))
	{
//		offset = get_offset(rawdata);
		AC_VOLTAGE = AC_Get_Voltage(rawdata);
	}

}
