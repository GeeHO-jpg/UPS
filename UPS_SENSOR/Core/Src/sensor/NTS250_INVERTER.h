/*
 * NTP250_INVERTER.h
 *
 *  Created on: Apr 16, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_SENSOR_NTS250_INVERTER_H_
#define SRC_SENSOR_NTS250_INVERTER_H_

#include <stdint.h>
#include <string.h>
#include "../comm/uart_comm.h"


/*
b0 inverter_mode            = เครื่องอยู่ใน Inverter mode; ค่า 1 หมายถึงอยู่โหมดอินเวอร์เตอร์.
b1 bypass_mode              = อยู่ใน Bypass mode; ค่า 1 หมายถึง bypass ทำงาน.
b2 utility_present          = มี utility power หรือไฟ AC mains เข้ามา; ค่า 1 หมายถึงมีไฟ utility.
b3 utility_charger_enable   = utility charger enable; ค่า 1 หมายถึงวงจรชาร์จจาก utility ถูก enable.
b4 solar_charger_enable     = solar charger enable; ค่า 1 หมายถึงวงจรชาร์จจากโซลาร์ถูก enable.
b5 saving_mode              = อยู่ใน saving mode / power saving mode; ค่า 1 หมายถึงกำลังอยู่โหมดประหยัดพลังงาน.
b6 battery_exhausted        = แบตอ่อน/ใกล้หมด; ค่า 1 หมายถึง battery low.
b7 shutdown_mode            = shutdown เพราะแบตใช้หมด; คู่มืออธิบายว่าเป็น shutdown mode จาก battery used up.
b8 battery_ovp              = Battery over-voltage protection; ค่า 1 หมายถึงเกิด battery OVP.
b9 remote_shutdown          = ถูก สั่งปิดจากรีโมต/คำสั่งสื่อสาร; ค่า 1 หมายถึง remote shutdown.
b10 olp_100_115             = overload อยู่ช่วง 100–115%; ค่า 1 หมายถึงเกิด overload ในช่วงนี้.
b11 olp_115_150             = overload อยู่ช่วง 115–150%; ค่า 1 หมายถึงเกิด overload ในช่วงนี้.
b12 olp_over_150            = overload มากกว่า 150%; ค่า 1 หมายถึงเกิด overload ระดับนี้.
b13 otp                     = over-temperature protection; คู่มือระบุพิเศษว่า 1 = inverter OTP และ 2 = fan lock protection. ตัวนี้จึงไม่ควรมองเป็นแค่ boolean อย่างเดียว.
b14 inv_uvp                 = inverter undervoltage protection; ค่า 1 หมายถึง INV UVP protection.
b15 inv_ovp                 = inverter overvoltage protection; ค่า 1 หมายถึง INV OVP protection.
b16 inv_fault               = internal inverter fault; ค่า 1 หมายถึง inverter fault.
b17 eeprom_error            = EEPROM error; ค่า 1 หมายถึง EEPROM error.
b18 system_shutdown         = system shutdown; ค่า 1 หมายถึงระบบปิดตัวลงแล้ว.

*/
typedef struct
{
    uint8_t inverter_mode;
    uint8_t bypass_mode;
    uint8_t utility_present;
    uint8_t utility_charger_enable;
    uint8_t solar_charger_enable;
    uint8_t saving_mode;
    uint8_t battery_exhausted;
    uint8_t shutdown_mode;
    uint8_t battery_ovp;
    uint8_t remote_shutdown;
    uint8_t olp_100_115;
    uint8_t olp_115_150;
    uint8_t olp_over_150;
    uint8_t otp;
    uint8_t inv_uvp;
    uint8_t inv_ovp;
    uint8_t inv_fault;
    uint8_t eeprom_error;
    uint8_t system_shutdown;
} QStatusBits_t;

typedef struct
{
    uint16_t output_voltage_ac;     // VVV
    uint8_t  load_percent_digital;  // QQQ
    float    battery_voltage;       // SS.S
    uint8_t  battery_percent;      // BBB
    float    heatsink_temp;         // TT.T
    uint16_t utility_voltage;       // MMM
    float    output_frequency;      // RR.R
    uint16_t dc_bus_voltage;        // DDD
    uint8_t  load_percent_analog;   // PPP

    QStatusBits_t status;
} InverterQData_t;


uint8_t NTS250_Q_CMD_Request(void);
uint8_t NTS250_Parse_Q_CMD(const uint8_t *raw, uint16_t len, InverterQData_t *out);
uint8_t NTS250_GetQData(InverterQData_t *out);

void NTS250_init(Uart_Comm_id_t uart_id);
void NTS250_Task(void);

#endif /* SRC_SENSOR_NTS250_INVERTER_H_ */
