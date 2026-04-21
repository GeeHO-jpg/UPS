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

extern volatile uint16_t master_tx_complete;
extern volatile uint16_t master_rx_complete;
extern volatile uint16_t master_rx_complete_flag;
extern volatile uint16_t master_error_count;
extern volatile uint8_t  master_tx_busy;
extern volatile uint8_t master_rx_busy;

uint8_t i2ccom_run(void);
void i2ccom_read_from_slave(void);
void set_slave_address(uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif /* SRC_I2C_COMM_H_ */
