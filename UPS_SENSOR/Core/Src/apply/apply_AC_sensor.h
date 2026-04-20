/*
 * apply_AC_sensor.h
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_APPLY_AC_SENSOR_H_
#define SRC_APPLY_APPLY_AC_SENSOR_H_

#include <stdint.h>
#include "../sensor/AC_sensor.h"

uint8_t AC_app_init();
uint8_t AC_app_run(uint8_t *out);

#endif /* SRC_APPLY_APPLY_AC_SENSOR_H_ */
