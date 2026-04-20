/*
 * uart_comm.c
 */
#include "uart_comm.h"
#include <stdint.h>
#include "comm_def.h"

#if USE_UART == 1
#include "stm32f1xx_hal_uart.h"

typedef struct
{
    UART_HandleTypeDef *huart;
    volatile uint8_t tx_busy;
    volatile uint8_t rx_busy;
    volatile uint8_t tx_done;
    volatile uint8_t rx_done;
    volatile uint8_t error;
} Uart_Comm_t;

static Uart_Comm_t X_uart[UART_COMM_MAX];

static int32_t UART_FindIndex(UART_HandleTypeDef *huart)
{
    uint8_t i;

    if (huart == NULL)
    {
        return -1;
    }

    for (i = 0U; i < UART_COMM_MAX; i++)
    {
        if (X_uart[i].huart == huart)
        {
            return (int32_t)i;
        }
    }

    return -1;
}

uint8_t UART_ID_Register(Uart_Comm_id_t id, UART_HandleTypeDef *huart)
{
    if ((id >= UART_COMM_MAX) || (huart == NULL))
    {
        return 0U;
    }

    X_uart[id].huart   = huart;
    X_uart[id].tx_busy = 0U;
    X_uart[id].rx_busy = 0U;
    X_uart[id].tx_done = 0U;
    X_uart[id].rx_done = 0U;
    X_uart[id].error   = 0U;
    return 1U;
}

uint8_t UART_Comm_Transmit_IT(Uart_Comm_id_t id, const uint8_t *buf, uint16_t len)
{
    if ((id >= UART_COMM_MAX) || (buf == NULL) || (len == 0U) || (X_uart[id].huart == NULL))
    {
        return 0U;
    }

    if ((X_uart[id].tx_busy != 0U) || (X_uart[id].rx_busy != 0U))
    {
        return 0U;
    }

    X_uart[id].tx_done = 0U;
    X_uart[id].error   = 0U;
    X_uart[id].tx_busy = 1U;

    if (HAL_UART_Transmit_IT(X_uart[id].huart, (uint8_t *)buf, len) != HAL_OK)
    {
        X_uart[id].tx_busy = 0U;
        X_uart[id].error   = 1U;
        return 0U;
    }

    return 1U;
}

uint8_t UART_Comm_Receive_IT(Uart_Comm_id_t id, uint8_t *buf, uint16_t len)
{
    if ((id >= UART_COMM_MAX) || (buf == NULL) || (len == 0U) || (X_uart[id].huart == NULL))
    {
        return 0U;
    }

    if ((X_uart[id].tx_busy != 0U) || (X_uart[id].rx_busy != 0U))
    {
        return 0U;
    }

    X_uart[id].rx_done = 0U;
    X_uart[id].error   = 0U;
    X_uart[id].rx_busy = 1U;

    if (HAL_UART_Receive_IT(X_uart[id].huart, buf, len) != HAL_OK)
    {
        X_uart[id].rx_busy = 0U;
        X_uart[id].error   = 1U;
        return 0U;
    }

    return 1U;
}

uint8_t UART_Comm_Abort(Uart_Comm_id_t id)
{
    if ((id >= UART_COMM_MAX) || (X_uart[id].huart == NULL))
    {
        return 0U;
    }

    (void)HAL_UART_Abort(X_uart[id].huart);

    X_uart[id].tx_busy = 0U;
    X_uart[id].rx_busy = 0U;
    X_uart[id].tx_done = 0U;
    X_uart[id].rx_done = 0U;
    X_uart[id].error   = 0U;
    return 1U;
}

uint8_t UART_Comm_IsTxDone(Uart_Comm_id_t id)
{
    if (id >= UART_COMM_MAX)
    {
        return 0U;
    }

    return X_uart[id].tx_done;
}

uint8_t UART_Comm_IsRxDone(Uart_Comm_id_t id)
{
    if (id >= UART_COMM_MAX)
    {
        return 0U;
    }

    return X_uart[id].rx_done;
}

uint8_t UART_Comm_IsBusy(Uart_Comm_id_t id)
{
    if (id >= UART_COMM_MAX)
    {
        return 0U;
    }

    return (uint8_t)((X_uart[id].tx_busy != 0U) || (X_uart[id].rx_busy != 0U));
}

uint8_t UART_Comm_HasError(Uart_Comm_id_t id)
{
    if (id >= UART_COMM_MAX)
    {
        return 0U;
    }

    return X_uart[id].error;
}

void UART_Comm_ClearFlags(Uart_Comm_id_t id)
{
    if (id >= UART_COMM_MAX)
    {
        return;
    }

    X_uart[id].tx_done = 0U;
    X_uart[id].rx_done = 0U;
    X_uart[id].error   = 0U;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    int32_t idx = UART_FindIndex(huart);

    if (idx < 0)
    {
        return;
    }

    X_uart[idx].tx_busy = 0U;
    X_uart[idx].tx_done = 1U;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    int32_t idx = UART_FindIndex(huart);

    if (idx < 0)
    {
        return;
    }

    X_uart[idx].rx_busy = 0U;
    X_uart[idx].rx_done = 1U;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    int32_t idx = UART_FindIndex(huart);

    if (idx < 0)
    {
        return;
    }

    X_uart[idx].tx_busy = 0U;
    X_uart[idx].rx_busy = 0U;
    X_uart[idx].error   = 1U;
}

uint8_t UART_TransmitBlocking(Uart_Comm_id_t id, const uint8_t *tx_buf, uint16_t tx_len, uint32_t timeout_ms)
{
    if ((id >= UART_COMM_MAX) || (X_uart[id].huart == NULL) || (tx_buf == NULL))
    {
        return 0U;
    }

    if (HAL_UART_Transmit(X_uart[id].huart, (uint8_t *)tx_buf, tx_len, timeout_ms) != HAL_OK)
    {
        return 0U;
    }

    return 1U;
}

uint8_t UART_ReceiveBlocking(Uart_Comm_id_t id, uint8_t *rx_buf, uint16_t rx_len, uint32_t timeout_ms)
{
    if ((id >= UART_COMM_MAX) || (X_uart[id].huart == NULL) || (rx_buf == NULL) || (rx_len == 0U))
    {
        return 0U;
    }

    if (HAL_UART_Receive(X_uart[id].huart, rx_buf, rx_len, timeout_ms) != HAL_OK)
    {
        return 0U;
    }

    return 1U;
}

#endif
