#ifndef SRC_SENSOR_DC_SENSOR_H_
#define SRC_SENSOR_DC_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../comm/uart_comm.h"


typedef struct
{
    uint16_t voltage_x100;   // 100.00V => 10000
    uint16_t current_x100;   // 1.00A   => 100
    uint32_t power_x10;      // 100.0W  => 1000
    uint32_t energy_wh;      // 0Wh     => 0

    uint16_t high_alarm_status;
    uint16_t low_alarm_status;
} DC_SensorRawValue_t;

uint8_t PZEM003_DC_ParseRawValue(const uint8_t *rx, DC_SensorRawValue_t *out);
uint8_t PZEM003_DC_Sensor_Request(void);
uint8_t PZEM003_DC_Sensor_GetRaw(uint8_t *out, uint16_t len);

void PZEM003_DC_Sensor_Init(Uart_Comm_id_t uart_id, UART_HandleTypeDef *hart);
void PZEM003_DC_Sensor_Task(void);


#ifdef __cplusplus
}
#endif

#endif
