/*
 * adc_comm.h
 *
 *  Created on: Apr 2, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_ADC_COMM_H_
#define SRC_APPLY_ADC_COMM_H_

#include "stdint.h"
#include "stdint.h"
#include "main.h"

typedef enum{
	ADC_COMM_1 = 1,
	ADC_COMM_2,
	ADC_COMM_MAX
}ADC_COMM_ID_t;

//initial
uint8_t ADC_ID_Register(ADC_COMM_ID_t id,ADC_HandleTypeDef *hadc);
uint8_t ADC_Comm_Start(ADC_COMM_ID_t id);

//loop
uint8_t ADC_Comm_GetLatest(ADC_COMM_ID_t id,volatile int16_t *out);

#endif /* SRC_APPLY_ADC_COMM_H_ */
