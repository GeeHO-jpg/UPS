#include "app.h"
#include <string.h>
#include <stdbool.h>
#include "ring_buffer.h"
#include "warp_function.h"
#include "i2c_comm.h"
#include "../Packet_RCSA/UDPPacket.h"
#include "../Packet_RCSA/UDPPacketHeader.h"
#include "../Packet_RCSA/common_defs.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"

#define RB_size 512

#define max_buffer 64
#define slave_wind_addr 0x01
#define slave_env_addr 0x02
#define slave_ups_powwer_addr 0x03


/* external ring buffers */
RingBuffer i2c_tx_slave_rb;
RingBuffer i2c_rx_slave_rb;
RingBuffer i2c_tx_master_rb;
RingBuffer i2c_rx_master_rb;

//extern I2C_HandleTypeDef hi2c2;
uint8_t payload_buffer[max_buffer];
uint8_t data_extrack[max_buffer];
static uint32_t payload_loop = 0;
static uint8_t first_send_done = 0U;
//static CommandType current_cmd = CMD_HANDSHAKE;
//static CommandType current_cmd = CMD_STATION_WIND_SENSOR;
//static CommandType current_cmd = CMD_STATION_ENV_SENSOR;

static uint8_t dbg_wind_raw[max_buffer];
static uint8_t dbg_env_raw[max_buffer];
static uint8_t dbg_ups_raw[max_buffer];

static uint16_t dbg_wind_len = 0U;
static uint16_t dbg_env_len  = 0U;
static uint16_t dbg_ups_len  = 0U;

//static uint32_t dbg_wind_tick = 0U;
//static uint32_t dbg_env_tick  = 0U;
//static uint32_t dbg_ups_tick  = 0U;


bool led_flag = false;

/* debug variable */

int8_t wind_speed = 0;
//float wind_speed = 0.0f;
float direct_wind = 0.0f;
uint16_t raw_direct_wind = 0;
int8_t raw_wind_speed = 0;
//uint16_t raw_wind_speed = 0;

float hum_env = 0.0f;
int8_t temp_env = 0;
//float temp_env = 0.0f;
float press_env = 0.0f;
uint16_t raw_hum_env = 0;
int8_t raw_temp_env = 0;
//uint16_t raw_temp_env = 0;
uint16_t raw_press_env = 0;


/*
 * ups
 * */
uint8_t grid_power =0;
uint8_t  upsstationActive = 0;
float  battery_Backup = 0.0f;        //(0-120)/10vdc
uint8_t AC_Voltage = 0;				//(0-220)Vac
uint8_t AC_Current = 0;
uint16_t raw_CurrentDC = 0;
uint16_t raw_VoltageDC = 0;
float chargCurrentDC = 0;        //(0-5000)/100 A
float chargVoltageDC = 0;        //(0-20000)/100 V
/*--------------------------------------------*/
slave_id_t slave_sq = SLAVE_WIND_SENSOR;

static void extract_payload_data(uint16_t len, const uint8_t *data_fromPacket, uint8_t *data_out, uint16_t data_out_size)
{
    if ((data_out == NULL) || (data_fromPacket == NULL) || (data_out_size == 0U))
    {
        return;
    }

    if (len > data_out_size)
    {
        len = data_out_size;
    }

    memset(data_out, 0, data_out_size);
    memcpy(data_out, data_fromPacket, len);
}


static void check_pack(UDPPacket *pkt)
{
//    uint16_t copy_len;

    if ((pkt == NULL) || (pkt->header == NULL) || (pkt->payload == NULL))
    {
        return;
    }

    switch (pkt->header->cmd)
    {
        case CMD_HANDSHAKE:
        	extract_payload_data(pkt->header->payload_size, pkt->payload,payload_buffer,sizeof(payload_buffer));
            first_send_done = 1U;
            break;

        case CMD_STATION_WIND_SENSOR:
        	extract_payload_data(pkt->header->payload_size, pkt->payload,dbg_wind_raw,sizeof(dbg_wind_raw));

        	dbg_wind_len = pkt->header->payload_size;

        	raw_direct_wind = ((uint16_t)dbg_wind_raw[0] << 8) | dbg_wind_raw[1];
        	raw_wind_speed  = ((int8_t)dbg_wind_raw[2]);

        	wind_speed = raw_wind_speed;
        	direct_wind = (float)raw_direct_wind/10;
            break;


        case CMD_STATION_ENV_SENSOR:
            extract_payload_data(pkt->header->payload_size,pkt->payload,dbg_env_raw,sizeof(dbg_env_raw));
            dbg_env_len = pkt->header->payload_size;

            if (dbg_env_len < 15U)
            {
                break;
            }

            memcpy(&hum_env, &dbg_env_raw[0], sizeof(float));

            raw_temp_env  = (int8_t)dbg_env_raw[4];
//            raw_noise_env = ((uint16_t)dbg_env_raw[5] << 8) | dbg_env_raw[6];
            raw_press_env = ((uint16_t)dbg_env_raw[9] << 8) | dbg_env_raw[10];


            temp_env  = (float)raw_temp_env;


            press_env = (float)raw_press_env / 10.0f;

            break;

//        case CMD_STATION_ENV_SENSOR:
//        	extract_payload_data(pkt->header->payload_size, pkt->payload,dbg_env_raw,sizeof(dbg_env_raw));
//        	dbg_env_len = pkt->header->payload_size;
//
//        	raw_hum_env = ((uint16_t)dbg_env_raw[0] << 8) | dbg_env_raw[1];
//        	raw_temp_env  = (int8_t)dbg_env_raw[2];
//        	raw_press_env = ((uint16_t)dbg_env_raw[7] << 8) | dbg_env_raw[8];
//
//
//
//        	hum_env = (float)raw_hum_env/10;
//        	temp_env = raw_temp_env;
//        	press_env = (float)raw_press_env/10;
//            break;



        case CMD_STATION_POWER_UPS_SENSOR:
        	extract_payload_data(pkt->header->payload_size, pkt->payload,dbg_ups_raw,sizeof(dbg_ups_raw));
        	dbg_ups_len = pkt->header->payload_size;

        	grid_power = dbg_ups_raw[0];
        	upsstationActive = dbg_ups_raw[1];
        	battery_Backup = ((float)dbg_ups_raw[2])/10.0f;        //(0-120)/10vdc
        	AC_Voltage = dbg_ups_raw[3];				//(0-220)Vac
        	AC_Current = dbg_ups_raw[4];
        	raw_CurrentDC = ((uint16_t)dbg_ups_raw[5] << 8) | dbg_ups_raw[6];        //(0-5000)/100 A
        	chargCurrentDC = ((float)raw_CurrentDC)/100.0f;


        	raw_VoltageDC = ((uint16_t)dbg_ups_raw[7] << 8) | dbg_ups_raw[8];

        	chargVoltageDC = ((float)raw_VoltageDC)/100.0f;        //(0-20000)/100 V

        	break;
        default:
            break;
    }
}

static uint16_t create_payload(uint8_t *payload, uint8_t len_you_need)
{
    uint8_t i;

    for (i = 0; i < len_you_need; i++)
    {
//        payload[i] = (uint8_t)(i + payload_loop);
    	payload[i] = (uint8_t)(i);
    }

    payload_loop++;
    return len_you_need;
}





uint8_t App_Master_CreatePack(uint8_t cmd,const uint8_t *payload,uint16_t payload_len){
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

    /* warp_function.c ควรเป็นคน serialize packet ลง ring ตาม role */
    SendUDPPacketI2CComm(pkt);

    FreeUDPPacket(pkt);
    return 1U;

}

static uint8_t app_master_rb_init(void)
 {
     if (rb_init(&i2c_tx_master_rb, RB_size) != 0)
     {
         return 0U;
     }

     if (rb_init(&i2c_rx_master_rb, RB_size) != 0)
     {
         return 0U;
     }

     return 1U;
 }

void Master_app_init(void)
 {
     if (app_master_rb_init() == 0U)
     {
         Error_Handler();
     }


     InitializeI2CComm(I2CMasterReadByte, I2CMasterWriteByte);
 }


static uint8_t Master_BuildAndSendRequest(CommandType req)
{
    uint16_t payload_len = 0U;

    switch (req)
    {
        case CMD_HANDSHAKE:
            if (first_send_done == 0U)
            {
                payload_len = create_payload(payload_buffer, 4U);
            }
            else
            {
                payload_len = 4U;   /* ใช้ค่าที่รับกลับมาแล้วใน payload_buffer */
            }

            if (payload_len == 0U)
            {
                return 0U;
            }

            return App_Master_CreatePack(CMD_HANDSHAKE,  payload_buffer,   payload_len);

        case CMD_STATION_WIND_SENSOR:
            payload_buffer[0] = 0x01U;
            payload_len = 1U;

            return App_Master_CreatePack(CMD_STATION_WIND_SENSOR, payload_buffer, payload_len);

        case CMD_STATION_ENV_SENSOR:
            payload_buffer[0] = 0x01U;
            payload_len = 1U;

            return App_Master_CreatePack(CMD_STATION_ENV_SENSOR, payload_buffer, payload_len);

        case CMD_STATION_POWER_UPS_SENSOR:
            payload_buffer[0] = 0x01U;
            payload_len = 1U;

            return App_Master_CreatePack(CMD_STATION_POWER_UPS_SENSOR, payload_buffer, payload_len);
        default:
            return 0U;
    }
}

void Master_app_run(void)
{
    uint32_t now = HAL_GetTick();
    static uint32_t last = 0U;
//    static uint32_t pending_tick = 0U;
    static uint16_t last_error_count = 0U;
    static uint8_t request_pending = 0U;
    UDPPacket *pkt;
    uint8_t build_ok = 0U;

    RunReceiveI2CComm();

    pkt = GetReceivedUDPPacketI2CComm();
    if (pkt != NULL)
    {
        check_pack(pkt);
        FreeUDPPacket(pkt);
    }

    if ((request_pending == 0U) && ((now - last) >= 300U))
    {
        last = now;
        switch (slave_sq)
        {
        case SLAVE_WIND_SENSOR:
			set_slave_address((uint8_t)slave_wind_addr);
			build_ok = Master_BuildAndSendRequest(CMD_STATION_WIND_SENSOR);
			slave_sq = SLAVE_ENV_SENSOR;
			break;

		case SLAVE_ENV_SENSOR:
			set_slave_address((uint8_t)slave_env_addr);
			build_ok = Master_BuildAndSendRequest(CMD_STATION_ENV_SENSOR);
			slave_sq = SLAVE_UPS_SUPPLY;
			break;

		case SLAVE_UPS_SUPPLY:
			set_slave_address((uint8_t)slave_ups_powwer_addr);
			build_ok = Master_BuildAndSendRequest(CMD_STATION_POWER_UPS_SENSOR);   // หรือ CMD ของ UPS จริง
			slave_sq = SLAVE_WIND_SENSOR;
			break;

        default:
        	slave_sq = SLAVE_WIND_SENSOR;
            break;
        }




        if (build_ok != 0U)
        {
//        	i2ccom_run();
//			request_pending = 1U;
//			pending_tick = now;

            if (i2ccom_run() != 0U)
            {
                request_pending = 1U;
//                pending_tick = now;
            }
        }

    }

    if (master_rx_complete != 0U)
    {
        master_rx_complete = 0U;
        request_pending = 0U;

        led_flag = !led_flag;
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,led_flag ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    /* ถ้ามี error เกิดขึ้น ให้ปลด pending */
    if (master_error_count != last_error_count)
    {
        last_error_count = master_error_count;
        request_pending = 0U;
    }

//    /* กันค้างถ้า callback ไม่กลับมา */
//    if ((request_pending != 0U) && ((now - pending_tick) > 200U))
//    {
//        request_pending = 0U;
//        master_tx_busy = 0U;
//        master_rx_busy = 0U;
//        master_error_count++;
//
//        HAL_I2C_DeInit(&hi2c1);
//        HAL_I2C_Init(&hi2c1);
//    }
}
// void Master_app_run(void)
// {
//     uint32_t now = HAL_GetTick();
//     static uint32_t last = 0U;
//     static uint32_t pending_tick = 0U;
//     static uint16_t last_error_count = 0U;
//     static uint8_t request_pending = 0U;
//     UDPPacket *pkt;

//     RunReceiveI2CComm();

//     pkt = GetReceivedUDPPacketI2CComm();
//     if (pkt != NULL)
//     {
//         check_pack(pkt);
//         FreeUDPPacket(pkt);
//     }

//     if ((request_pending == 0U) && ((now - last) >= 100U))
//     {
//         last = now;

//         if (Master_BuildAndSendRequest(current_cmd) != 0U)
//         {
//             if (i2ccom_run() != 0U)
//             {
//                 request_pending = 1U;
//                 pending_tick = now;
//             }
//         }
//     }

//     if (master_rx_complete != 0U)
//     {
//         master_rx_complete = 0U;
//         request_pending = 0U;

//         led_flag = !led_flag;
//         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,led_flag ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     }

//     /* ถ้ามี error เกิดขึ้น ให้ปลด pending */
//     if (master_error_count != last_error_count)
//     {
//         last_error_count = master_error_count;
//         request_pending = 0U;
//     }

//     /* กันค้างถ้า callback ไม่กลับมา */
//     if ((request_pending != 0U) && ((now - pending_tick) > 200U))
//     {
//         request_pending = 0U;
//         master_tx_busy = 0U;
//         master_rx_busy = 0U;
//         master_error_count++;

//         HAL_I2C_DeInit(&hi2c1);
//         HAL_I2C_Init(&hi2c1);
//     }
// }
