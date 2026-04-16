/*
 * i2c_comm.h
 */

#ifndef SRC_I2C_COMM_H_
#define SRC_I2C_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_i2c.h"
#include <stdint.h>

extern volatile uint16_t slave_tx_complete;
extern volatile uint16_t slave_rx_complete;
extern volatile uint16_t slave_error_count;
extern volatile uint8_t slave_tx_busy;
extern volatile uint8_t slave_rx_busy;
extern volatile uint8_t get_from_master_flag;


#ifdef __cplusplus
}
#endif

#endif /* SRC_I2C_COMM_H_ */
