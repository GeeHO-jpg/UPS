#include "app.h"
#include <string.h>
#include <stdbool.h>
#include "main.h"
#include "ring_buffer.h"
#include "warp_function.h"
#include "../comm/i2c_comm.h"
#include "../Packet_RCSA/UDPPacket.h"
#include "../Packet_RCSA/UDPPacketHeader.h"
#include "../Packet_RCSA/common_defs.h"
#include "../sensor/AC_sensor.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_i2c.h"
#include "stm32f1xx_hal_adc.h"

typedef struct{
    uint8_t  gridPower;             //1-0
    uint8_t  upsstationActive;      //1-0
    uint8_t  battery_Backup;        //(0-120)/10vdc

    uint8_t AC_Voltage;				//(0-220)Vac
    uint8_t AC_Current;

    uint8_t  charging;              //1-0
    uint8_t chargepercent; 			//
    uint16_t chargCurrentDC;        //(0-500)/10 A
    uint16_t chargVoltageDC;        //(0-2000)/10 V

}ups_data_t;



#define RB_size 512

RingBuffer i2c_tx_slave_rb;
RingBuffer i2c_rx_slave_rb;
RingBuffer i2c_tx_master_rb;
RingBuffer i2c_rx_master_rb;

//ZMPT101B ACsensor ={0};
ups_data_t UPS_data = {0};

static uint32_t last = 0U;
uint8_t payLoad_CMD[64];
uint8_t reply_payLoad_CMD[64];
uint8_t payLoad_sensor[4];
bool led_state = false;
bool packet_received_flag = false;

extern ADC_HandleTypeDef hadc1;
extern volatile uint8_t get_from_master_flag;
extern I2C_HandleTypeDef hi2c1;
extern volatile uint8_t slave_need_relisten;







static uint16_t App_BuildEnvPayload(const ups_data_t *src, uint8_t *dst, uint16_t dst_size)
{
    if ((src == NULL) || (dst == NULL) || (dst_size < 11U))
    {
        return 0U;
    }

    dst[0]  = (uint8_t)(src->gridPower);

    dst[1]  = (uint8_t)(src->upsstationActive);

    dst[2]  = (uint8_t)(src->charging);

    dst[3]  = (uint8_t)(src->battery_Backup);


    dst[4]  = (uint8_t)(src->AC_Voltage);
    dst[5]  = (uint8_t)(src->AC_Current);

    dst[6] = (uint8_t)(src->chargCurrentDC >> 8);
    dst[7] = (uint8_t)(src->chargCurrentDC & 0xFFU);


    dst[8] = (uint8_t)(src->chargVoltageDC >> 8);
    dst[9] = (uint8_t)(src->chargVoltageDC & 0xFFU);

    dst[10] = (uint8_t)(src -> chargepercent);



    return 11U;
}


static uint8_t App_Slave_CreatePack(uint8_t cmd, const uint8_t *payload, uint16_t payload_len)
{
    UDPPacketHeader *h;
    UDPPacket *pkt;

    if ((payload == NULL) || (payload_len == 0U))
    {
        return 0U;
    }

    h = CreateUDPPacketHeader(0U, cmd, payload_len);
    if (h == NULL)
    {
        return 0U;
    }

    pkt = CreateUDPPacket(h);
    if (pkt == NULL)
    {
        free(h);
        return 0U;
    }

    if (!AttachCompletedPayload(pkt, payload, payload_len))
    {
        FreeUDPPacket(pkt);
        return 0U;
    }

    SendUDPPacketI2CComm(pkt);
    FreeUDPPacket(pkt);
    return 1U;
}

static uint8_t App_Slave_SendEnvSensorPacket(void)
{
    uint8_t payload[14];
    uint16_t payload_len;

    payload_len = App_BuildEnvPayload(&UPS_data, payload, sizeof(payload));
    if (payload_len == 0U)
    {
        return 0U;
    }

    return App_Slave_CreatePack(CMD_STATION_POWER_UPS_SENSOR, payload, payload_len);
}

static void make_plus_one_payload(const uint8_t *src, uint8_t *dst, uint16_t len)
{
    uint16_t i;

    for (i = 0U; i < len; i++)
    {
        dst[i] = (uint8_t)(src[i] + 1U);
    }
}
static void check_packet(UDPPacket *pkt)
{
    uint16_t copy_len;

    if ((pkt == NULL) || (pkt->header == NULL) || (pkt->payload == NULL))
    {
        return;
    }

    switch (pkt->header->cmd)
    {
        case CMD_HANDSHAKE:
            copy_len = pkt->header->payload_size;
            if (copy_len > sizeof(payLoad_CMD))
            {
                copy_len = sizeof(payLoad_CMD);
            }

            memcpy(payLoad_CMD, pkt->payload, copy_len);
            packet_received_flag = true;

            make_plus_one_payload(pkt->payload, reply_payLoad_CMD, copy_len);
            (void)App_Slave_CreatePack(CMD_HANDSHAKE, reply_payLoad_CMD, copy_len);
            break;


        case CMD_STATION_POWER_UPS_SENSOR:
            if ((pkt->header->payload_size >= 1U) && (pkt->payload[0] == 0x01U))
            {
                (void)App_Slave_SendEnvSensorPacket();
            }
            break;

        default:
            break;
    }
}




static uint8_t app_slave_rb_init(void)
{
    if (rb_init(&i2c_tx_slave_rb, RB_size) != 0)
    {
        return 0U;
    }

    if (rb_init(&i2c_rx_slave_rb, RB_size) != 0)
    {
        return 0U;
    }

    return 1U;
}

void Slave_app_init(void)
{
    if (app_slave_rb_init() == 0U)
    {
        Error_Handler();
    }

//    ZMPT101B_init(&ACsensor, &hadc1, 50);
    InitializeI2CComm(I2CSlaveReadByte, I2CSlaveWriteByte);
    HAL_I2C_EnableListen_IT(&hi2c1);
}

void Slave_app_run(void)
{
    uint32_t now = HAL_GetTick();
    UDPPacket *pkt;
    static uint32_t sensor_last = 0U;

    if ((now - sensor_last) >= 300U)
    {
        sensor_last = now;
//		ZMPT101B_getZeroPoint(&ACsensor, adc_AC_raw);
//		UPS_data.AC_Voltage = ZMPT101B_getRmsVoltage(&ACsensor, adc_AC_raw);

    }


//    (void)EnvSensor_GetLatest(&ENV_payload_sensor, 1000U);

    RunReceiveI2CComm();

    pkt = GetReceivedUDPPacketI2CComm();
    if (pkt != NULL)
    {
        check_packet(pkt);
        FreeUDPPacket(pkt);
    }

    if ((now - last) >= 100U)
    {
    	if(slave_tx_complete>=10){
    		slave_tx_complete = 0;
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

//        HAL_I2C_DeInit(&hi2c1);
//        HAL_I2C_Init(&hi2c1);
        HAL_I2C_EnableListen_IT(&hi2c1);
    }
}




