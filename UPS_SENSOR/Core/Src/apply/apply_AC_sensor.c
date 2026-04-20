/*
 * apply_AC_sensor.c
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#include "apply_AC_sensor.h"

#include <stdint.h>
#include <main.h>


#include "../comm/adc_comm.h"


#define FREQUENCY 50
#define SENSITIVITY 1000.0f
extern ADC_HandleTypeDef hadc1;

volatile int16_t rawdata = 0;
volatile int16_t offset = 0;
// volatile uint8_t AC_VOLTAGE = 0;

static uint8_t comm_init()
{
	if(!ADC_ID_Register(ADC_COMM_1,&hadc1)){
        return 0;
    }
	if(!ADC_Comm_Start(ADC_COMM_1))
    {
        return 0;
    }
	return 1U;
}

uint8_t AC_app_init()
{
	if(!comm_init())
    {
        return 0;
    }
	AC_init(FREQUENCY,SENSITIVITY);
    return 1;
}

uint8_t AC_app_run(uint8_t *out)
{
    if (out == NULL)
    {
        return 0U;
    }

    if (ADC_Comm_GetLatest(ADC_COMM_1, &rawdata) != 0U)
    {
        *out = AC_Get_Voltage(rawdata);
        
        return 1U;
    }

    return 0U;
}
