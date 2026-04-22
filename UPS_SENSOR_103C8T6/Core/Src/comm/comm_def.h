/*
 * comm_def.h
 *
 *  Created on: Apr 7, 2026
 *      Author: _TTTXN
 */

#ifndef SRC_COMM_COMM_DEF_H_
#define SRC_COMM_COMM_DEF_H_

#define USE_UART 1
#define USE_ADC  0
#define USE_I2C  1
#define USE_SPI  0




#if USE_UART == 1
/* UART config */
#define DATA_BUFFER 64
#define RAW_DATA_SIZE 21
#endif

#if USE_ADC == 1
/* ADC config */
#endif

#if USE_I2C == 1
/* I2C config */
#define MAX_BUF 64
#endif

#if USE_SPI == 1
/* SPI config */
#endif

#endif /* SRC_COMM_COMM_DEF_H_ */
