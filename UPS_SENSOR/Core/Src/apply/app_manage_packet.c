/*
 * app_manage_packet.c
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#include "app_manage_packet.h"
#include <stdint.h>
#include <string.h>
#include <main.h>
#include "../Packet_RCSA/UDPPacket.h"
#include "../Packet_RCSA/UDPPacketHeader.h"
#include "../Packet_RCSA/common_defs.h"

#include "apply_AC_sensor.h"
#include "apply_DC_sensor.h"
#include "apply_INVERTER.h"

#include "../warp_rb/warp_function.h"
ups_data_t UPSPacket;

/*sensor variable*/
PZEM003Values_t DC_out;
InverterQData_t inv_out;
PZEMValues AC_DATA;

uint8_t payLoad_CMD[64];
uint8_t reply_payLoad_CMD[64];
bool packet_received_flag = false;



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

static uint16_t App_BuildPayload(const ups_data_t *src, uint8_t *dst, uint16_t dst_size)
{
    if ((src == NULL) || (dst == NULL) || (dst_size < 9U))
    {
        return 0U;
    }

    dst[0]  = (uint8_t)(src->gridPower);

    dst[1]  = (uint8_t)(src->upsstationActive);

    dst[2]  = (uint8_t)(src->battery_Backup);


    dst[3]  = (uint8_t)(src->AC_Voltage);
    dst[4]  = (uint8_t)(src->AC_Current);

    dst[5] = (uint8_t)(src->chargCurrentDC >> 8);
    dst[6] = (uint8_t)(src->chargCurrentDC & 0xFFU);


    dst[7] = (uint8_t)(src->chargVoltageDC >> 8);
    dst[8] = (uint8_t)(src->chargVoltageDC & 0xFFU);





    return 9U;
}

static uint8_t App_Slave_SendUPSSensorPacket(void)
{
    uint8_t payload[9];
    uint16_t payload_len;

    payload_len = App_BuildPayload(&UPSPacket, payload, sizeof(payload));
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

//static AppPacketType_t App_CheckPacket(const UDPPacket *pkt)
//{
//    if ((pkt == NULL) || (pkt->header == NULL))
//    {
//        return APP_PKT_INVALID;
//    }
//
//    if ((pkt->header->payload_size > 0U) && (pkt->payload == NULL))
//    {
//        return APP_PKT_INVALID;
//    }
//
//    switch (pkt->header->cmd)
//    {
//        case CMD_HANDSHAKE:
//        {
//            return APP_PKT_HANDSHAKE;
//        }
//
//        case CMD_STATION_POWER_UPS_SENSOR:
//        {
//            if ((pkt->header->payload_size >= 1U) && (pkt->payload[0] == 0x01U))
//            {
//                return APP_PKT_REQ_UPS_SENSOR;
//            }
//            return APP_PKT_INVALID;
//        }
//
//        default:
//        {
//            return APP_PKT_INVALID;
//        }
//    }
//}

static void app_pack_data_to_struct(void)
{
    if(AC_app_Run(&AC_DATA) != 0U)
    {
        // ได้ข้อมูล AC ใหม่
    	UPSPacket.AC_Voltage = (uint8_t)(AC_DATA.voltage_x10 / 10U);
    }
    if (DC_app_Run(&DC_out) != 0U)
    {
        // ได้ข้อมูล DC ใหม่
        UPSPacket.chargCurrentDC =  DC_out.current_x100;
        UPSPacket.chargVoltageDC =  DC_out.voltage_x100;
    }else
    {
        UPSPacket.chargCurrentDC =  0;
        UPSPacket.chargVoltageDC =  0;
    }

    if (NTS250_APP_run(&inv_out) != 0U)
    {
        // ได้ข้อมูล inverter ใหม่
    	UPSPacket.battery_Backup = (uint8_t)inv_out.battery_voltage_x10;
        
    }else{
    	UPSPacket.battery_Backup = 0;
    }

}

static uint8_t App_HandlePacket(const UDPPacket *pkt)
{

    uint16_t copy_len;



    switch (pkt->header->cmd)
    {
        case CMD_HANDSHAKE:
        {
            copy_len = pkt->header->payload_size;
            if (copy_len > sizeof(payLoad_CMD))
            {
                copy_len = sizeof(payLoad_CMD);
            }

            memcpy(payLoad_CMD, pkt->payload, copy_len);
            packet_received_flag = true;

            make_plus_one_payload(pkt->payload, reply_payLoad_CMD, copy_len);
            (void)App_Slave_CreatePack(CMD_HANDSHAKE, reply_payLoad_CMD, copy_len);

            return 1U;
        }

        case CMD_STATION_POWER_UPS_SENSOR:
        {
        	if ((pkt->header->payload_size >= 1U) && (pkt->payload[0] == 0x01U)){
        		(void)App_Slave_SendUPSSensorPacket();
        	}

            return 1U;
        }

        case CMD_COUNT:
        default:
        {
            return 0U;
        }
    }
}




void app_pack_run(void)
{
    UDPPacket *pkt;

    app_pack_data_to_struct();
    RunReceiveI2CComm();
    pkt = GetReceivedUDPPacketI2CComm();
    if (pkt != NULL)
    {
        App_HandlePacket(pkt);
        FreeUDPPacket(pkt);
    }
}
