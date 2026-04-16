#pragma once

#include <string.h>
#include <stm32f1xx_hal_uart.h>
#include <stdlib.h>
#include <stdint.h>


class PZEM03_DCVoltageSensor {
public:

    typedef struct{
        uint16_t voltage; // แรงดันไฟฟ้า
        uint16_t current; // กระแสไฟฟ้า
        uint16_t power;   // กำลังไฟฟ้า
        uint16_t energy;  // พลังงานไฟฟ้า
        uint16_t highVoltAlarm;
        uint16_t lowVoltAlarm;

    } SensorData;

    explicit PZEM03_DCVoltageSensor(UART_HandleTypeDef* uart) : _uart(uart) {}



    bool getdata(float& voltage) {
        uint8_t request[6] = {slaveAddress, read_input_Register, addressBeginHighByte, addressBeginLowByte, amountOfDataHighByte, amountOfDataLowByte}; 
        
        uint16_t crc = modbus_crc16(request, sizeof(request));
        uint8_t crcLowByte = crc & 0xFF;
        uint8_t crcHighByte = (crc >> 8) & 0xFF;
        uint8_t fullRequest[8];
        memcpy(fullRequest, request, sizeof(request));

        fullRequest[6] = crcLowByte;
        fullRequest[7] = crcHighByte;
        if (HAL_UART_Transmit(_uart, fullRequest, sizeof(fullRequest), HAL_MAX_DELAY) != HAL_OK) {
            return false; 
        }

        uint8_t responseFrame[64];
        uint8_t responseWithoutCRC[19];

        if (HAL_UART_Receive(_uart, responseFrame, sizeof(responseFrame), HAL_MAX_DELAY) != HAL_OK) {
            return false; 
        }

        memcpy(responseWithoutCRC, responseFrame, sizeof(responseWithoutCRC));
        uint16_t receivedCrc = ((responseFrame[19] << 8) && 0xFF) | responseFrame[20];
        uint16_t calculatedCrc = modbus_crc16(responseWithoutCRC, sizeof(responseWithoutCRC));
        if (receivedCrc != calculatedCrc) {
            return false; // CRC ไม่ถูกต้อง
        }

        // ตรวจสอบความถูกต้องของข้อมูลที่ได้รับ
        if (responseFrame[0] != slaveAddress || responseFrame[1] != read_input_Register ) {
            return false; // ข้อมูลไม่ถูกต้อง
        }

        // แปลงข้อมูลเป็นแรงดันไฟฟ้า
        // uint16_t rawVoltage = (responseFrame[3] << 8) | responseFrame[4];
        // voltage = rawVoltage / 10.0f; // สมมติว่าแรงดันไฟฟ้าถูกส่งมาในรูปแบบที่ต้องหารด้วย 10

        /*
        เหลือแปลงข้อมูลเป็นแรงดันไฟฟ้า โดยดูจากเอกสารของเซนเซอร์ว่าแรงดันไฟฟ้าถูกส่งมาในรูปแบบใด เช่น อาจจะเป็น 16-bit unsigned integer ที่ต้องหารด้วย 10 หรือ 100 เพื่อให้ได้ค่าแรงดันไฟฟ้าที่ถูกต้อง
        */

        return true; // อ่านแรงดันไฟฟ้าสำเร็จ
    }

    bool setHighVoltAlarm(){
        uint8_t cmd_setHighVoltAlarm[8] = {slaveAddress, Write_Single_Register, calibate_Register, 0x00, 0x00, 0x01, 0x00, 0x00}; // ตัวอย่างคำสั่งตั้งค่า High Voltage Alarm
    }



private:


    UART_HandleTypeDef* _uart;
    uint8_t slaveAddress = 0x00; //boardcast address
    uint8_t read_input_Register = 0x04;
    uint8_t Read_Holding_Register = 0x03;
    uint8_t Write_Single_Register = 0x06;
    uint8_t calibate_Register = 0x41;
    uint8_t energy_reset_Register = 0x42;

    uint16_t addressBegin = 0x0000;
    uint8_t addressBeginHighByte = (addressBegin >> 8) & 0xFF ;
    uint8_t addressBeginLowByte = addressBegin & 0xFF ;

    uint16_t amountOfData = 0x0008;
    uint8_t amountOfDataHighByte = (amountOfData >> 8) & 0xFF ;
    uint8_t amountOfDataLowByte = amountOfData & 0xFF ;

    uint16_t HighVoltAlarmRegister = 0x0000;
    uint8_t HighVoltAlarmHighByte = (HighVoltAlarmRegister >> 8) & 0xFF ;
    uint8_t HighVoltAlarmLowByte = HighVoltAlarmRegister & 0xFF ;

    uint16_t LowVoltAlarmRegister = 0x0001;
    uint8_t LowVoltAlarmHighByte = (LowVoltAlarmRegister >> 8) & 0xFF ;
    uint8_t LowVoltAlarmLowByte = LowVoltAlarmRegister & 0xFF ;

    uint16_t ModbusRTURegister = 0x0002;
    uint8_t ModbusRTUHighByte = (ModbusRTURegister >> 8) & 0xFF ;
    uint8_t ModbusRTULowByte = ModbusRTURegister & 0xFF ;

    uint16_t HighVoltAlarmTHD = 0x1D4C; // 1 LSB=0.01V, 75V; 75/0.01 = 7500 ,7500 = 0x1D4C
    uint8_t HighVoltAlarmTHDHighByte = (HighVoltAlarmTHD >> 8) & 0xFF ;
    uint8_t HighVoltAlarmTHDLowByte = HighVoltAlarmTHD & 0xFF ;    

    uint16_t LowVoltAlarmTHD = 0x1194; // 1 LSB=0.01V, 45V; 45/0.01 = 4500 ,4500 = 0x1194
    uint8_t LowVoltAlarmTHDHighByte = (LowVoltAlarmTHD >> 8) & 0xFF ;
    uint8_t LowVoltAlarmTHDLowByte = LowVoltAlarmTHD & 0xFF ;    

    uint16_t modbus_crc16(const uint8_t* data, size_t length) {
        uint16_t crc = 0xFFFF;

        for (size_t i = 0; i < length; i++) {
            crc ^= data[i];

            for (int j = 0; j < 8; j++) {
                if (crc & 0x0001) {
                    crc >>= 1;
                    crc ^= 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }

        return crc;
    }


    
};

