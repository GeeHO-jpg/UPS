#ifndef SRC_APPLY_APP_H_
#define SRC_APPLY_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "ring_buffer.h"

/* app role */

typedef enum{
	SLAVE_EDLE=0,
    SLAVE_WIND_SENSOR =1,
    SLAVE_ENV_SENSOR = 2,
    SLAVE_UPS_SUPPLY = 3,
}slave_id_t;

extern RingBuffer i2c_tx_slave_rb;
extern RingBuffer i2c_rx_slave_rb;
extern RingBuffer i2c_tx_master_rb;
extern RingBuffer i2c_rx_master_rb;

/* app api */
void Master_app_init(void);
void Master_app_run(void);

#ifdef __cplusplus
}
#endif

#endif /* SRC_APPLY_APP_H_ */
