#ifndef SRC_APPLY_APP_H_
#define SRC_APPLY_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include "ring_buffer.h"

/* app role */

extern RingBuffer i2c_tx_slave_rb;
extern RingBuffer i2c_rx_slave_rb;
extern RingBuffer i2c_tx_master_rb;
extern RingBuffer i2c_rx_master_rb;

/* app api */
void Slave_app_init(void);
void Slave_app_run(void);

#ifdef __cplusplus
}
#endif

#endif /* SRC_APPLY_APP_H_ */
