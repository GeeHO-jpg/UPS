///*
// * NTS250_INVERTER.c
// */
//#include "NTS250_INVERTER.h"
//#include "../comm/uart_comm.h"
//#include <string.h>
////#include <stdlib.h>
//
//
//#define NTS250_CMD_PERIOD_MS   800U
//#define NTS250_RX_TIMEOUT_MS   200U
//#define NTS250_FRAME_MAX_LEN   64U
//
//#define NTS250_STATE_IDLE      0U
//#define NTS250_STATE_WAIT_TX   1U
//#define NTS250_STATE_WAIT_RX   2U
//
//static const uint8_t NTS250_Q_CMD[2] = { 'Q', '\r' };
//
//typedef struct
//{
//    Uart_Comm_id_t id;
//    uint8_t state;
//    uint8_t valid;
//    uint8_t rx_byte;
//    uint16_t rx_len;
//    uint32_t last_req_tick;
//    uint32_t last_rx_tick;
//    uint8_t rx_buf[NTS250_FRAME_MAX_LEN];
//    char clean_buf[NTS250_FRAME_MAX_LEN];
//    InverterQData_t data;
//} NTS250_Context_t;
//
//static NTS250_Context_t nts;
//
//
//static uint16_t NTS250_ParseU16(const char *s)
//{
//    uint16_t v = 0U;
//
//    if (s == NULL)
//    {
//        return 0U;
//    }
//
//    while ((*s >= '0') && (*s <= '9'))
//    {
//        v = (uint16_t)((v * 10U) + (uint16_t)(*s - '0'));
//        s++;
//    }
//
//    return v;
//}
//
//static uint8_t NTS250_ParseU8(const char *s)
//{
//    return (uint8_t)NTS250_ParseU16(s);
//}
//
//static int16_t NTS250_ParseS16_X10(const char *s)
//{
//    int16_t sign = 1;
//    int16_t value = 0;
//    uint8_t frac_done = 0U;
//
//    if (s == NULL)
//    {
//        return 0;
//    }
//
//    if (*s == '-')
//    {
//        sign = -1;
//        s++;
//    }
//
//    while (*s != '\0')
//    {
//        if ((*s >= '0') && (*s <= '9'))
//        {
//            value = (int16_t)(value * 10 + (int16_t)(*s - '0'));
//            if (frac_done != 0U)
//            {
//                break;
//            }
//        }
//        else if (*s == '.')
//        {
//            frac_done = 1U;
//        }
//        else
//        {
//            break;
//        }
//        s++;
//    }
//
//    if (frac_done == 0U)
//    {
//        value = (int16_t)(value * 10);
//    }
//
//    return (int16_t)(value * sign);
//}
//
//static uint16_t NTS250_ParseU16_X10(const char *s)
//{
//    int16_t v = NTS250_ParseS16_X10(s);
//
//    if (v < 0)
//    {
//        return 0U;
//    }
//
//    return (uint16_t)v;
//}
//
//static void NTS250_ClearData(void)
//{
//    memset(&nts.data, 0, sizeof(nts.data));
//    nts.valid = 0U;
//}
//
//static void NTS250_ResetRx(void)
//{
//    nts.rx_len = 0U;
//    memset(nts.rx_buf, 0, sizeof(nts.rx_buf));
//    memset(nts.clean_buf, 0, sizeof(nts.clean_buf));
//}
//
//static void NTS250_ResetComm(void)
//{
//    (void)UART_Comm_Abort(nts.id);
//    UART_Comm_ClearFlags(nts.id);
//    nts.state = NTS250_STATE_IDLE;
//    NTS250_ResetRx();
//}
//
//static uint8_t NTS250_StripFrame(const uint8_t *raw, uint16_t raw_len, char *out, uint16_t out_size)
//{
//    uint16_t end;
//    uint16_t copy_len;
//
//    if ((raw == NULL) || (out == NULL) || (raw_len < 4U) || (out_size < 2U))
//    {
//        return 0U;
//    }
//
//    if (raw[0] != '(')
//    {
//        return 0U;
//    }
//
//    end = raw_len;
//    while ((end > 0U) && ((raw[end - 1U] == '\r') || (raw[end - 1U] == '\n')))
//    {
//        end--;
//    }
//
//    if ((end == 0U) || (raw[end - 1U] != ')') || (end <= 2U))
//    {
//        return 0U;
//    }
//
//    copy_len = (uint16_t)(end - 2U);
//    if ((copy_len + 1U) > out_size)
//    {
//        return 0U;
//    }
//
//    memcpy(out, &raw[1], copy_len);
//    out[copy_len] = '\0';
//    return 1U;
//}
//
//static uint8_t NTS250_ParseStatusBits(const char *bits, QStatusBits_t *st)
//{
//    uint8_t i;
//
//    if ((bits == NULL) || (st == NULL) || (strlen(bits) != 19U))
//    {
//        return 0U;
//    }
//
//    for (i = 0U; i < 19U; i++)
//    {
//        if ((bits[i] != '0') && (bits[i] != '1'))
//        {
//            return 0U;
//        }
//    }
//
//
//    st->inverter_mode          = (uint8_t)(bits[0]  - '0');
//    st->bypass_mode            = (uint8_t)(bits[1]  - '0');
//    st->utility_present        = (uint8_t)(bits[2]  - '0');
//    st->utility_charger_enable = (uint8_t)(bits[3]  - '0');
//    st->solar_charger_enable   = (uint8_t)(bits[4]  - '0');
//    st->saving_mode            = (uint8_t)(bits[5]  - '0');
//    st->battery_exhausted      = (uint8_t)(bits[6]  - '0');
//    st->shutdown_mode          = (uint8_t)(bits[7]  - '0');
//    st->battery_ovp            = (uint8_t)(bits[8]  - '0');
//    st->remote_shutdown        = (uint8_t)(bits[9]  - '0');
//    st->olp_100_115            = (uint8_t)(bits[10] - '0');
//    st->olp_115_150            = (uint8_t)(bits[11] - '0');
//    st->olp_over_150           = (uint8_t)(bits[12] - '0');
//    st->otp                    = (uint8_t)(bits[13] - '0');
//    st->inv_uvp                = (uint8_t)(bits[14] - '0');
//    st->inv_ovp                = (uint8_t)(bits[15] - '0');
//    st->inv_fault              = (uint8_t)(bits[16] - '0');
//    st->eeprom_error           = (uint8_t)(bits[17] - '0');
//    st->system_shutdown        = (uint8_t)(bits[18] - '0');
//    return 1U;
//}
//
//static uint8_t NTS250_ParsePayload(char *clean, InverterQData_t *out)
//{
//    char *token;
//    char *tokens[10];
//    uint8_t count = 0U;
//
//    if ((clean == NULL) || (out == NULL))
//    {
//        return 0U;
//    }
//
//    memset(out, 0, sizeof(*out));
//
//    token = strtok(clean, " ");
//    while ((token != NULL) && (count < 10U))
//    {
//        tokens[count++] = token;
//        token = strtok(NULL, " ");
//    }
//
//    if (count != 10U)
//    {
//        return 0U;
//    }
//
//
//
//
//    out->output_voltage_ac    = NTS250_ParseU16(tokens[0]);
//    out->load_percent_digital = NTS250_ParseU8(tokens[1]);
//    out->battery_voltage_x10  = NTS250_ParseU16_X10(tokens[2]);
//    out->battery_percent      = NTS250_ParseU8(tokens[3]);
//    out->heatsink_temp_x10    = NTS250_ParseS16_X10(tokens[4]);
//    out->utility_voltage      = NTS250_ParseU16(tokens[5]);
//    out->output_frequency_x10 = NTS250_ParseU16_X10(tokens[6]);
//    out->dc_bus_voltage       = NTS250_ParseU16(tokens[7]);
//    out->load_percent_analog  = NTS250_ParseU8(tokens[8]);
//
//    return NTS250_ParseStatusBits(tokens[9], &out->status);
//}
//
//static uint8_t NTS250_StartRequest(void)
//{
//    if ((nts.id >= UART_COMM_MAX) || (nts.state != NTS250_STATE_IDLE) || (UART_Comm_IsBusy(nts.id) != 0U))
//    {
//        return 0U;
//    }
//
//    UART_Comm_ClearFlags(nts.id);
//    NTS250_ResetRx();
//
//    if (UART_Comm_Transmit_IT(nts.id, NTS250_Q_CMD, sizeof(NTS250_Q_CMD)) == 0U)
//    {
//        return 0U;
//    }
//
//    nts.state = NTS250_STATE_WAIT_TX;
//    nts.last_req_tick = HAL_GetTick();
//    return 1U;
//}
//
//void NTS250_init(Uart_Comm_id_t uart_id)
//{
//    memset(&nts, 0, sizeof(nts));
//    nts.id = uart_id;
//    nts.state = NTS250_STATE_IDLE;
//    NTS250_ClearData();
//}
//
//void NTS250_Task(void)
//{
//    uint32_t now;
//
//    if (nts.id >= UART_COMM_MAX)
//    {
//        return;
//    }
//
//    now = HAL_GetTick();
//
//    if (UART_Comm_HasError(nts.id) != 0U)
//    {
//        NTS250_ResetComm();
//        NTS250_ClearData();
//    }
//
//    if ((nts.state == NTS250_STATE_IDLE) && ((now - nts.last_req_tick) >= NTS250_CMD_PERIOD_MS))
//    {
//        (void)NTS250_StartRequest();
//        return;
//    }
//
//    if ((nts.state == NTS250_STATE_WAIT_TX) && (UART_Comm_IsTxDone(nts.id) != 0U))
//    {
//        UART_Comm_ClearFlags(nts.id);
//
//        if (UART_Comm_Receive_IT(nts.id, &nts.rx_byte, 1U) != 0U)
//        {
//            nts.state = NTS250_STATE_WAIT_RX;
//            nts.last_rx_tick = now;
//        }
//        else
//        {
//            NTS250_ResetComm();
//            NTS250_ClearData();
//        }
//        return;
//    }
//
//    if (nts.state != NTS250_STATE_WAIT_RX)
//    {
//        return;
//    }
//
//    if ((now - nts.last_rx_tick) > NTS250_RX_TIMEOUT_MS)
//    {
//        NTS250_ResetComm();
//        NTS250_ClearData();
//        return;
//    }
//
//    if (UART_Comm_IsRxDone(nts.id) == 0U)
//    {
//        return;
//    }
//
//    UART_Comm_ClearFlags(nts.id);
//    nts.last_rx_tick = now;
//
//    if (nts.rx_len >= (NTS250_FRAME_MAX_LEN - 1U))
//    {
//        NTS250_ResetComm();
//        NTS250_ClearData();
//        return;
//    }
//
//    nts.rx_buf[nts.rx_len++] = nts.rx_byte;
//
//    if (nts.rx_byte != '\r')
//    {
//        if (UART_Comm_Receive_IT(nts.id, &nts.rx_byte, 1U) == 0U)
//        {
//            NTS250_ResetComm();
//            NTS250_ClearData();
//        }
//        return;
//    }
//
//    nts.state = NTS250_STATE_IDLE;
//    nts.rx_buf[nts.rx_len] = '\0';
//
//    if (NTS250_StripFrame(nts.rx_buf, nts.rx_len, nts.clean_buf, sizeof(nts.clean_buf)) == 0U)
//    {
//        NTS250_ClearData();
//        return;
//    }
//
//    if (NTS250_ParsePayload(nts.clean_buf, &nts.data) == 0U)
//    {
//        NTS250_ClearData();
//        return;
//    }
//
//    nts.valid = 1U;
//}
//
//uint8_t NTS250_GetLatest(InverterQData_t *out)
//{
//    if (out == NULL)
//    {
//        return 0U;
//    }
//
//    if (nts.valid == 0U)
//    {
//        memset(out, 0, sizeof(*out));
//        return 0U;
//    }
//
//    *out = nts.data;
//    return 1U;
//}
