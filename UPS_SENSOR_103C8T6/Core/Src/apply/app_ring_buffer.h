/*
 * app_ring_buffer.h
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_APP_RING_BUFFER_H_
#define SRC_APPLY_APP_RING_BUFFER_H_

#include "../warp_rb/ring_buffer.h"
#include "../warp_rb/warp_function.h"

extern RingBuffer i2c_tx_slave_rb;
extern RingBuffer i2c_rx_slave_rb;
extern RingBuffer i2c_tx_master_rb;
extern RingBuffer i2c_rx_master_rb;

uint8_t app_slave_rb_init(void);


#endif /* SRC_APPLY_APP_RING_BUFFER_H_ */
