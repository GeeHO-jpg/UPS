/*
 * NTS250P_INVERTER.c
 *
 *  Created on: Apr 22, 2026
 *      Author: _TTTXN
 */


#include "NTS250P_INVERTER.h"
#include "../comm/uart_comm.h"
#include <string.h>
#include <stdint.h>


#define NTS250_CMD_PERIOD_MS      1000U
#define NTS250_TIMEOUT_MS         200U
#define NTS250_Q_RESP_LEN         61U
#define NTS250_CLEAN_BUF_LEN      64U

typedef enum
{
    NTS250_STATE_IDLE = 0,
    NTS250_STATE_SEND_REQ,
    NTS250_STATE_WAIT_TX_DONE,
    NTS250_STATE_START_RX,
    NTS250_STATE_WAIT_RX_DONE,
    NTS250_STATE_PARSE,
    NTS250_STATE_ERROR
} NTS250_State_t;

static const uint8_t NTS250_Q_CMD[2] = { 'Q', '\r' };

typedef struct
{
    Uart_Comm_id_t id;
    NTS250_State_t state;
    uint8_t valid;
    uint32_t state_tick;
    uint32_t last_poll_tick;
    uint8_t rx_buf[NTS250_Q_RESP_LEN + 1U];
    char clean_buf[NTS250_CLEAN_BUF_LEN];
    InverterQData_t latest;
} NTS250_Context_t;

static NTS250_Context_t nts;

static uint16_t NTS250_ParseU16(const char *s)
{
    uint16_t v = 0U;

    if (s == NULL)
    {
        return 0U;
    }

    while ((*s >= '0') && (*s <= '9'))
    {
        v = (uint16_t)((v * 10U) + (uint16_t)(*s - '0'));
        s++;
    }

    return v;
}

static uint8_t NTS250_ParseU8(const char *s)
{
    return (uint8_t)NTS250_ParseU16(s);
}

static int16_t NTS250_ParseS16_X10(const char *s)
{
    int16_t sign = 1;
    int16_t value = 0;
    uint8_t has_dot = 0U;

    if (s == NULL)
    {
        return 0;
    }

    if (*s == '-')
    {
        sign = -1;
        s++;
    }

    while (*s != '\0')
    {
        if ((*s >= '0') && (*s <= '9'))
        {
            value = (int16_t)(value * 10 + (int16_t)(*s - '0'));
            if (has_dot != 0U)
            {
                break;
            }
        }
        else if (*s == '.')
        {
            has_dot = 1U;
        }
        else
        {
            break;
        }
        s++;
    }

    if (has_dot == 0U)
    {
        value = (int16_t)(value * 10);
    }

    return (int16_t)(value * sign);
}

static uint16_t NTS250_ParseU16_X10(const char *s)
{
    int16_t v = NTS250_ParseS16_X10(s);

    if (v < 0)
    {
        return 0U;
    }

    return (uint16_t)v;
}

static void NTS250_ClearLatest(void)
{
    memset(&nts.latest, 0, sizeof(nts.latest));
    nts.valid = 0U;
}

static void NTS250_ClearRxBuffer(void)
{
    memset(nts.rx_buf, 0, sizeof(nts.rx_buf));
    memset(nts.clean_buf, 0, sizeof(nts.clean_buf));
}

static void NTS250_ResetComm(void)
{
    (void)UART_Comm_Abort(nts.id);
    UART_Comm_ClearFlags(nts.id);
    NTS250_ClearRxBuffer();
    nts.state = NTS250_STATE_IDLE;
}

static uint8_t NTS250_StripFrame(const uint8_t *raw, uint16_t raw_len, char *out, uint16_t out_size)
{
    uint16_t end;
    uint16_t copy_len;

    if ((raw == NULL) || (out == NULL) || (raw_len < 4U) || (out_size < 2U))
    {
        return 0U;
    }

    if (raw[0] != '(')
    {
        return 0U;
    }

    end = raw_len;
    while ((end > 0U) && ((raw[end - 1U] == '\r') || (raw[end - 1U] == '\n') || (raw[end - 1U] == '\0')))
    {
        end--;
    }

    if ((end == 0U) || (raw[end - 1U] != ')') || (end <= 2U))
    {
        return 0U;
    }

    copy_len = (uint16_t)(end - 2U);
    if ((copy_len + 1U) > out_size)
    {
        return 0U;
    }

    memcpy(out, &raw[1], copy_len);
    out[copy_len] = '\0';
    return 1U;
}

static uint8_t NTS250_ParseStatusBits(const char *bits, QStatusBits_t *st)
{
    uint8_t i;

    if ((bits == NULL) || (st == NULL) || (strlen(bits) != 19U))
    {
        return 0U;
    }

    for (i = 0U; i < 19U; i++)
    {
        if ((bits[i] != '0') && (bits[i] != '1'))
        {
            return 0U;
        }
    }

    st->inverter_mode          = (uint8_t)(bits[0]  - '0');
    st->bypass_mode            = (uint8_t)(bits[1]  - '0');
    st->utility_present        = (uint8_t)(bits[2]  - '0');
    st->utility_charger_enable = (uint8_t)(bits[3]  - '0');
    st->solar_charger_enable   = (uint8_t)(bits[4]  - '0');
    st->saving_mode            = (uint8_t)(bits[5]  - '0');
    st->battery_exhausted      = (uint8_t)(bits[6]  - '0');
    st->shutdown_mode          = (uint8_t)(bits[7]  - '0');
    st->battery_ovp            = (uint8_t)(bits[8]  - '0');
    st->remote_shutdown        = (uint8_t)(bits[9]  - '0');
    st->olp_100_115            = (uint8_t)(bits[10] - '0');
    st->olp_115_150            = (uint8_t)(bits[11] - '0');
    st->olp_over_150           = (uint8_t)(bits[12] - '0');
    st->otp                    = (uint8_t)(bits[13] - '0');
    st->inv_uvp                = (uint8_t)(bits[14] - '0');
    st->inv_ovp                = (uint8_t)(bits[15] - '0');
    st->inv_fault              = (uint8_t)(bits[16] - '0');
    st->eeprom_error           = (uint8_t)(bits[17] - '0');
    st->system_shutdown        = (uint8_t)(bits[18] - '0');
    return 1U;
}

static uint8_t NTS250_ParsePayload(char *clean, InverterQData_t *out)
{
    char *token;
    char *tokens[10];
    uint8_t count = 0U;

    if ((clean == NULL) || (out == NULL))
    {
        return 0U;
    }

    memset(out, 0, sizeof(*out));

    token = strtok(clean, " ");
    while ((token != NULL) && (count < 10U))
    {
        tokens[count++] = token;
        token = strtok(NULL, " ");
    }

    if (count != 10U)
    {
        return 0U;
    }

    out->output_voltage_ac    = NTS250_ParseU16(tokens[0]);
    out->load_percent_digital = NTS250_ParseU8(tokens[1]);
    out->battery_voltage_x10  = NTS250_ParseU16_X10(tokens[2]);
    out->battery_percent      = NTS250_ParseU8(tokens[3]);
    out->heatsink_temp_x10    = NTS250_ParseS16_X10(tokens[4]);
    out->utility_voltage      = NTS250_ParseU16(tokens[5]);
    out->output_frequency_x10 = NTS250_ParseU16_X10(tokens[6]);
    out->dc_bus_voltage       = NTS250_ParseU16(tokens[7]);
    out->load_percent_analog  = NTS250_ParseU8(tokens[8]);

    return NTS250_ParseStatusBits(tokens[9], &out->status);
}

void NTS250_init(Uart_Comm_id_t uart_id, UART_HandleTypeDef *huart)
{
    memset(&nts, 0, sizeof(nts));
    nts.id = uart_id;
    nts.state = NTS250_STATE_IDLE;

    UART_ID_Register(uart_id, huart);
    NTS250_ClearLatest();
    NTS250_ClearRxBuffer();
}

void NTS250_Task(void)
{
    switch (nts.state)
    {
        case NTS250_STATE_IDLE:
        {
            if (nts.id >= UART_COMM_MAX)
            {
                break;
            }

            if ((HAL_GetTick() - nts.last_poll_tick) < NTS250_CMD_PERIOD_MS)
            {
                break;
            }

            if (UART_Comm_IsBusy(nts.id) != 0U)
            {
                break;
            }

            UART_Comm_ClearFlags(nts.id);
            nts.state = NTS250_STATE_SEND_REQ;
            break;
        }

        case NTS250_STATE_SEND_REQ:
        {
            NTS250_ClearRxBuffer();

            if (UART_Comm_Transmit_IT(nts.id, NTS250_Q_CMD, sizeof(NTS250_Q_CMD)) == 0U)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            nts.state_tick = HAL_GetTick();
            nts.state = NTS250_STATE_WAIT_TX_DONE;
            break;
        }

        case NTS250_STATE_WAIT_TX_DONE:
        {
            if (UART_Comm_HasError(nts.id) != 0U)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            if ((HAL_GetTick() - nts.state_tick) > NTS250_TIMEOUT_MS)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            if (UART_Comm_IsTxDone(nts.id) != 0U)
            {
                UART_Comm_ClearFlags(nts.id);
                nts.state = NTS250_STATE_START_RX;
            }
            break;
        }

        case NTS250_STATE_START_RX:
        {
            if (UART_Comm_Receive_IT(nts.id, nts.rx_buf, NTS250_Q_RESP_LEN) == 0U)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            nts.state_tick = HAL_GetTick();
            nts.state = NTS250_STATE_WAIT_RX_DONE;
            break;
        }

        case NTS250_STATE_WAIT_RX_DONE:
        {
            if (UART_Comm_HasError(nts.id) != 0U)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            if ((HAL_GetTick() - nts.state_tick) > NTS250_TIMEOUT_MS)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            if (UART_Comm_IsRxDone(nts.id) != 0U)
            {
                nts.state = NTS250_STATE_PARSE;
            }
            break;
        }

        case NTS250_STATE_PARSE:
        {
            UART_Comm_ClearFlags(nts.id);
            nts.rx_buf[NTS250_Q_RESP_LEN] = '\0';

            if (NTS250_StripFrame(nts.rx_buf, NTS250_Q_RESP_LEN, nts.clean_buf, sizeof(nts.clean_buf)) == 0U)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            if (NTS250_ParsePayload(nts.clean_buf, &nts.latest) == 0U)
            {
                nts.state = NTS250_STATE_ERROR;
                break;
            }

            nts.valid = 1U;
            nts.last_poll_tick = HAL_GetTick();
            nts.state = NTS250_STATE_IDLE;
            break;
        }

        case NTS250_STATE_ERROR:
        {
            NTS250_ResetComm();
            NTS250_ClearLatest();
            nts.last_poll_tick = HAL_GetTick();
            break;
        }

        default:
        {
            nts.state = NTS250_STATE_ERROR;
            break;
        }
    }
}

uint8_t NTS250_GetLatest(InverterQData_t *out)
{
    if (out == NULL)
    {
        return 0U;
    }

    if (nts.valid == 0U)
    {
        memset(out, 0, sizeof(*out));
        return 0U;
    }

    *out = nts.latest;
    return 1U;
}
