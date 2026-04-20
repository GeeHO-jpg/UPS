/*
 * apply.c
 *
 *  Created on: Apr 7, 2026
 *      Author: _TTTXN
 */


#include "apply.h"

#include "../comm/i2c_comm.h"
#include "apply_AC_sensor.h"
#include "apply_DC_sensor.h"
#include "apply_INVERTER.h"
#include "app_manage_packet.h"
#include "app_ring_buffer.h"


#include "main.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


static uint32_t last = 0U;
bool led_state = false;


extern I2C_HandleTypeDef hi2c1;
extern volatile uint8_t slave_need_relisten;

static uint8_t sensor_init(void)
{
    if (!AC_app_init())      return 0U;
    if (!dc_app_init())      return 0U;
    if (!NTS250_APP_init())  return 0U;
    return 1U;
}

void app_init(void)
{
    if (app_slave_rb_init() && sensor_init())
    {
        HAL_I2C_EnableListen_IT(&hi2c1);
    }
}

void app_run(void)
{
    uint32_t now = HAL_GetTick();

    app_pack_run();

    if ((now - last) >= 100U)
    {
    	if(slave_rx_complete>=10){
    		slave_rx_complete = 0;
            last = now;
            led_state = !led_state;
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, led_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    	}

    }
    if (slave_need_relisten != 0U)
    {
        slave_need_relisten = 0U;
        slave_tx_busy = 0U;
        slave_rx_busy = 0U;
        HAL_I2C_EnableListen_IT(&hi2c1);
    }

}
