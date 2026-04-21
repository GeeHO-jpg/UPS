#include "DC_sensor.h"
#include <string.h>

#define DC_REQ_LEN   8U
#define DC_RESP_LEN  21U

static Uart_Comm_id_t dc_id = UART_COMM_MAX;
static uint8_t dc_state = 0U;   /* 0=idle, 1=wait_tx, 2=wait_rx */
static uint8_t dc_ready = 0U;

static uint8_t tx_buf[DC_REQ_LEN];
static uint8_t rx_buf[DC_RESP_LEN];


static void PZEM003_DC_BuildCommand(void)
{
    tx_buf[0] = 0x01U;
    tx_buf[1] = 0x04U;
    tx_buf[2] = 0x00U;
    tx_buf[3] = 0x00U;
    tx_buf[4] = 0x00U;
    tx_buf[5] = 0x08U;
    tx_buf[6] = 0xF1U;
    tx_buf[7] = 0xCCU;
}


static uint16_t calcCRC(uint8_t *data, uint8_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }
    return crc;
}

static uint8_t PZEM003_CRC_CHECK(uint8_t *RAWdata, uint16_t len)
{
    uint16_t crc_calc;
    uint16_t crc_rx;
    crc_calc = calcCRC(RAWdata, len - 2U);

    /* Modbus CRC ส่ง Low byte ก่อน High byte */
    crc_rx = (uint16_t)RAWdata[len - 2U] | ((uint16_t)RAWdata[len - 1U] << 8);

    if (crc_calc != crc_rx)
    {
        return 0U;
    }

    return 1U;
}


//void DC_SetAddr()
//{
//
//}
//void DC_setHighVolte_Alarm()
//{
//
//}
//void DC_setLowVolte_Alarm()
//{
//
//}
//void DC_calibrate()
//{
//
//}
//
//void DC_resetPower()
//{
//
//}
uint8_t PZEM003_DC_ParseRawValue(const uint8_t *rx, DC_SensorRawValue_t *out)
{
    if ((rx == NULL) || (out == NULL))
    {
        return 0U;
    }

    if ((rx[0] != 0x01U) || (rx[1] != 0x04U) || (rx[2] != 0x10U))
    {
        return 0U;
    }

    out->voltage_x100 = ((uint16_t)rx[3] << 8) | rx[4];
    out->current_x100 = ((uint16_t)rx[5] << 8) | rx[6];

    out->power_x10 =
        (((uint32_t)rx[9]  << 8) | rx[10]) << 16 |
        (((uint32_t)rx[7]  << 8) | rx[8]);

    out->energy_wh =
        (((uint32_t)rx[13] << 8) | rx[14]) << 16 |
        (((uint32_t)rx[11] << 8) | rx[12]);

    out->high_alarm_status = ((uint16_t)rx[15] << 8) | rx[16];
    out->low_alarm_status  = ((uint16_t)rx[17] << 8) | rx[18];

    return 1U;
}



uint8_t PZEM003_DC_Sensor_Request(void)
{
    if (dc_id >= UART_COMM_MAX)
    {
        return 0U;
    }

    if (dc_state != 0U)
    {
        return 0U;
    }

    if (UART_Comm_IsBusy(dc_id) != 0U)
    {
        return 0U;
    }

    PZEM003_DC_BuildCommand();
    UART_Comm_ClearFlags(dc_id);
    dc_ready = 0U;

    if (UART_Comm_Transmit_IT(dc_id, tx_buf, DC_REQ_LEN) == 0U)
    {
        return 0U;
    }

    dc_state = 1U;
    return 1U;
}
void PZEM003_DC_Sensor_Init(Uart_Comm_id_t uart_id)
{
    dc_id = uart_id;
    dc_state = 0U;
    dc_ready = 0U;

    memset(tx_buf, 0, sizeof(tx_buf));
    memset(rx_buf, 0, sizeof(rx_buf));
    PZEM003_DC_Sensor_Request();
}

void PZEM003_DC_Sensor_Task(void)
{
    if (dc_id >= UART_COMM_MAX)
    {
        return;
    }

    if (UART_Comm_HasError(dc_id) != 0U)
    {
        UART_Comm_ClearFlags(dc_id);
        dc_state = 0U;
        return;
    }

    if (dc_state == 1U)
    {
        if (UART_Comm_IsTxDone(dc_id) != 0U)
        {
            UART_Comm_ClearFlags(dc_id);

            if (UART_Comm_Receive_IT(dc_id, rx_buf, DC_RESP_LEN) != 0U)
            {
                dc_state = 2U;
            }
            else
            {
                dc_state = 0U;
            }
        }
    }
    else if (dc_state == 2U)
    {
        if (UART_Comm_IsRxDone(dc_id) != 0U)
        {
            UART_Comm_ClearFlags(dc_id);
            dc_state = 0U;
            dc_ready = 1U;
        }
    }
}

uint8_t PZEM003_DC_Sensor_GetRaw(uint8_t *out, uint16_t len)
{
    if ((out == NULL) || (len < DC_RESP_LEN) || (dc_ready == 0U))
    {
        return 0U;
    }

    if(!(PZEM003_CRC_CHECK(rx_buf,DC_RESP_LEN))){
        return 0;
    }
    memcpy(out, rx_buf, DC_RESP_LEN);
    dc_ready = 0U;   /* อ่านแล้วเคลียร์ */
    return 1U;
}
