///*
// * AC_sensor.h
// *
// *  Created on: Mar 19, 2026
// *      Author: _TTTXN
// */
//
#ifndef SRC_SENSOR_AC_SENSOR_H_
#define SRC_SENSOR_AC_SENSOR_H_

#include "main.h"

void AC_init(uint8_t frequency, float scale);
//int16_t get_offset(int16_t raw_ADC_data);
uint8_t AC_Get_Voltage(uint16_t raw_adc_data);


#endif /* SRC_SENSOR_AC_SENSOR_H_ */
