/*
 * apply_INVERTER.h
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_APPLY_INVERTER_H_
#define SRC_APPLY_APPLY_INVERTER_H_

#include "../sensor/NTS250P_INVERTER.h"
#include <stdint.h>

uint8_t NTS250_APP_init();
uint8_t NTS250_APP_run(InverterQData_t *out);


#endif /* SRC_APPLY_APPLY_INVERTER_H_ */
