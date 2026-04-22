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
uint8_t AC_Get_Voltage(uint16_t raw_adc_data);


#endif /* SRC_SENSOR_AC_SENSOR_H_ */
