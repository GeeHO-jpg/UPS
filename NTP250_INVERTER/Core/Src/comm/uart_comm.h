/*
 * uart_comm.h
 */
#ifndef SRC_COMM_UART_COMM_H_
#define SRC_COMM_UART_COMM_H_

#include <stdint.h>
#include "main.h"

typedef enum
{
    UART_COMM_1 = 1,
    UART_COMM_2,
    UART_COMM_MAX
} Uart_Comm_id_t;

uint8_t UART_ID_Register(Uart_Comm_id_t id, UART_HandleTypeDef *huart);

uint8_t UART_Comm_Transmit_IT(Uart_Comm_id_t id, const uint8_t *buf, uint16_t len);
uint8_t UART_Comm_Receive_IT(Uart_Comm_id_t id, uint8_t *buf, uint16_t len);
uint8_t UART_Comm_Abort(Uart_Comm_id_t id);

uint8_t UART_Comm_IsTxDone(Uart_Comm_id_t id);
uint8_t UART_Comm_IsRxDone(Uart_Comm_id_t id);
uint8_t UART_Comm_IsBusy(Uart_Comm_id_t id);
uint8_t UART_Comm_HasError(Uart_Comm_id_t id);

void UART_Comm_ClearFlags(Uart_Comm_id_t id);

uint8_t UART_TransmitBlocking(Uart_Comm_id_t id, const uint8_t *tx_buf, uint16_t tx_len, uint32_t timeout_ms);
uint8_t UART_ReceiveBlocking(Uart_Comm_id_t id, uint8_t *rx_buf, uint16_t rx_len, uint32_t timeout_ms);

#endif
