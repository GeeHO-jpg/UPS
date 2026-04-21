# UPS_SENSOR Project

This project is designed for a UPS monitoring system based on **STM32F103C6T6**.  
It integrates multiple sensors and devices for AC, DC, and inverter monitoring, then packs the collected data into packets and sends them to the master through the **I2C protocol**.

## Modules

### 1. AC_SENSOR
- For **ZMPT101B**
- Uses **ADC protocol**
- Measures AC voltage from analog input

### 2. AC_PZEM004
- For **PZEM004**
- Uses **TTL protocol**
- Reads AC electrical parameters such as voltage, current, power, and energy

### 3. DC_SENSOR
- For **PZEM003**
- Uses **RS485 protocol**
- Reads DC electrical parameters such as voltage, current, and power  
- Requires an **RS485 to TTL module** for communication with the MCU

### 4. NTP250_INVERTER
- For **NTS250P-212 series**
- Uses **TTL protocol**
- Communicates with the inverter to read status and operating data

### 5. I2C_multi_master_103C8T6
- For **multi-address request handling via I2C**
- Used by the master to communicate with multiple slave devices

### 6. UPS_SENSOR
- Main application layer
- Collects data from multiple sensors
- Packs sensor data into a packet
- Sends the packet through **I2C protocol** with **address 0x03**

## Hardware Note
- **PZEM003** uses **RS485 communication**
- A **RS485 to TTL converter module** is required before connecting to STM32 UART

## Project Purpose
The purpose of this project is to create a compact UPS monitoring system that can:
- Read data from multiple power-related sensors
- Monitor both AC and DC parameters
- Read inverter information
- Send all collected data reliably to the main controller through I2C
