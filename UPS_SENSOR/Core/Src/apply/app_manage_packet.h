/*
 * app_manage_packet.h
 *
 *  Created on: Apr 17, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_APPLY_APP_MANAGE_PACKET_H_
#define SRC_APPLY_APP_MANAGE_PACKET_H_

#include <string.h>
#include <stdint.h>

#include "../Packet_RCSA/UDPPacket.h"

//typedef enum
//{
//    APP_PKT_INVALID = 0,
//    APP_PKT_HANDSHAKE,
//    APP_PKT_REQ_UPS_SENSOR
//} AppPacketType_t;

typedef struct{
    uint8_t  gridPower;             //1-0
    uint8_t  upsstationActive;      //1-0
    uint8_t  battery_Backup;        //(0-120)/10vdc

    uint8_t AC_Voltage;				//(0-220)Vac
    uint8_t AC_Current;

    // uint8_t  charging;              //1-0
    // uint8_t chargepercent; 			//
    uint16_t chargCurrentDC;        //(0-5000)/100 A
    uint16_t chargVoltageDC;        //(0-20000)/100 V

}ups_data_t;


void app_pack_run(void);

#endif /* SRC_APPLY_APP_MANAGE_PACKET_H_ */
