/*
 * apply_DC_sensor.h
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_APPLY_DC_SENSOR_H_
#define SRC_APPLY_APPLY_DC_SENSOR_H_
#include "../sensor/PZEM003.h"
//#include "../sensor/DC_sensor.h"

//uint8_t dc_app_init();
//uint8_t dc_app_run(DC_SensorRawValue_t *out);


uint8_t DC_app_init();
uint8_t DC_app_Run(PZEM003Values_t *out);

#endif /* SRC_APPLY_APPLY_DC_SENSOR_H_ */
