/*
 * apply_AC_sensor.h
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_APPLY_AC_SENSOR_H_
#define SRC_APPLY_APPLY_AC_SENSOR_H_

#include <stdint.h>
#include "../sensor/PZEM004.h"

uint8_t AC_app_init();
uint8_t AC_app_Run(PZEMValues *out);

#endif /* SRC_APPLY_APPLY_AC_SENSOR_H_ */
