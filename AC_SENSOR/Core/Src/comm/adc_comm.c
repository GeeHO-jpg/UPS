/*
 * adc_comm.c
 *
 *  Created on: Apr 2, 2026
 *      Author: _TTTXN
 */

#include "adc_comm.h"
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"


typedef struct
{
    ADC_HandleTypeDef *hadc;
    volatile int16_t latest_raw;

} ADC_CommCtx_t;

static ADC_CommCtx_t Xadc[ADC_COMM_MAX];

uint8_t ADC_ID_Register(ADC_COMM_ID_t id,ADC_HandleTypeDef *hadc){
    if ((id >= ADC_COMM_MAX) || (hadc == NULL)) {
        return 0U;
    }

    Xadc[id].hadc = hadc;
    Xadc[id].latest_raw = 0U;
	return 1U;
}

uint8_t ADC_Comm_Start(ADC_COMM_ID_t id)
{
    if (id >= ADC_COMM_MAX) {
        return 0U;
    }

    return (HAL_ADC_Start_IT(Xadc[id].hadc) == HAL_OK) ? 1U : 0U;
}

uint8_t ADC_Comm_GetLatest(ADC_COMM_ID_t id,volatile int16_t *out)
{
    if ((id >= ADC_COMM_MAX) || (out == NULL)) {
        return 0U;
    }

    *out = Xadc[id].latest_raw;
    return 1U;
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    uint8_t i;

    for (i = 0U; i < ADC_COMM_MAX; i++)
    {
        if (Xadc[i].hadc == hadc)
        {
        	Xadc[i].latest_raw = HAL_ADC_GetValue(hadc);
            (void)HAL_ADC_Start_IT(hadc);
            return;
        }
    }
}
