/*
 * NTP250_INVERTER.c
 *
 *  Created on: Apr 16, 2026
 *      Author: _TTTXN
 */

#include "NTS250_INVERTER.h"
#include "../comm/uart_comm.h"
#include <string.h>


#define DC_REQ_LEN   2U
#define DC_RESP_LEN  64U


static Uart_Comm_id_t NTS250_id = UART_COMM_MAX;
static uint8_t NTS250_state = 0U;   /* 0=idle, 1=wait_tx, 2=wait_rx */
static uint8_t NTS250_ready = 0U;

static uint8_t tx_buf[DC_REQ_LEN];
static uint8_t rx_buf[DC_RESP_LEN];

static uint8_t  rx_byte = 0U;       // รับทีละ 1 byte
static uint16_t rx_len  = 0U;       // ความยาวที่รับสะสมแล้ว

static InverterQData_t q_data;
static uint8_t q_data_valid = 0U;


static uint8_t IsDigitAscii(uint8_t c)
{
    return (uint8_t)((c >= '0') && (c <= '9'));
}

static uint8_t ParseU16_3(const uint8_t *s, uint16_t *out)
{
    if ((s == NULL) || (out == NULL))
    {
        return 0U;
    }

    if ((!IsDigitAscii(s[0])) || (!IsDigitAscii(s[1])) || (!IsDigitAscii(s[2])))
    {
        return 0U;
    }

    *out = (uint16_t)((uint16_t)(s[0] - '0') * 100U) +
           (uint16_t)((uint16_t)(s[1] - '0') * 10U) +
           (uint16_t)(s[2] - '0');
    return 1U;
}

static uint8_t ParseU8_3(const uint8_t *s, uint8_t *out)
{
    uint16_t temp = 0U;

    if ((s == NULL) || (out == NULL))
    {
        return 0U;
    }

    if (ParseU16_3(s, &temp) == 0U)
    {
        return 0U;
    }

    if (temp > 255U)
    {
        return 0U;
    }

    *out = (uint8_t)temp;
    return 1U;
}

static uint8_t ParseFloatX10_4(const uint8_t *s, float *out)
{
    uint16_t x10 = 0U;

    if ((s == NULL) || (out == NULL))
    {
        return 0U;
    }

    if ((!IsDigitAscii(s[0])) || (!IsDigitAscii(s[1])) || (s[2] != '.') || (!IsDigitAscii(s[3])))
    {
        return 0U;
    }

    x10 = (uint16_t)((uint16_t)(s[0] - '0') * 100U) +
          (uint16_t)((uint16_t)(s[1] - '0') * 10U) +
          (uint16_t)(s[3] - '0');

    *out = ((float)x10) / 10.0f;
    return 1U;
}

static void Create_Q_CMD(uint8_t *out)
{
 out[0] = 0x51;
 out[1] = 0x0D;
}





uint8_t NTS250_Parse_Q_CMD(const uint8_t *raw, uint16_t len, InverterQData_t *out)
{
    uint16_t temp_u16 = 0U;
    uint8_t i;

    if ((raw == NULL) || (out == NULL))
    {
        return 0U;
    }

    /* รองรับกรณีลงท้าย CRLF */
    if ((len > 0U) && (raw[len - 1U] == 0x0AU))
    {
        len--;
    }

    /* เฟรม Q ปกติ:
       (VVV QQQ SS.S BBB TT.T MMM RR.R DDD PPP b0...b18)\r
       index:
       0     = '('
       1..3  = VVV
       4     = ' '
       5..7  = QQQ
       8     = ' '
       9..12 = SS.S
       13    = ' '
       14..16= BBB
       17    = ' '
       18..21= TT.T
       22    = ' '
       23..25= MMM
       26    = ' '
       27..30= RR.R
       31    = ' '
       32..34= DDD
       35    = ' '
       36..38= PPP
       39    = ' '
       40..58= b0..b18
       59    = ')'
       60    = '\r'
    */
    if (len != 61U)
    {
        return 0U;
    }

    if ((raw[0] != '(') || (raw[59] != ')') || (raw[60] != 0x0DU))
    {
        return 0U;
    }

    if ((raw[4]  != ' ') || (raw[8]  != ' ') || (raw[13] != ' ') ||
        (raw[17] != ' ') || (raw[22] != ' ') || (raw[26] != ' ') ||
        (raw[31] != ' ') || (raw[35] != ' ') || (raw[39] != ' '))
    {
        return 0U;
    }

    memset(out, 0, sizeof(*out));

    if (ParseU16_3(&raw[1], &out->output_voltage_ac) == 0U)
    {
        return 0U;
    }

    if (ParseU8_3(&raw[5], &out->load_percent_digital) == 0U)
    {
        return 0U;
    }

    if (ParseFloatX10_4(&raw[9], &out->battery_voltage) == 0U)
    {
        return 0U;
    }

    if (ParseU8_3(&raw[14], &out->battery_percent) == 0U)
    {
        return 0U;
    }

    if (ParseFloatX10_4(&raw[18], &out->heatsink_temp) == 0U)
    {
        return 0U;
    }

    if (ParseU16_3(&raw[23], &out->utility_voltage) == 0U)
    {
        return 0U;
    }

    if (ParseFloatX10_4(&raw[27], &out->output_frequency) == 0U)
    {
        return 0U;
    }

    if (ParseU16_3(&raw[32], &out->dc_bus_voltage) == 0U)
    {
        return 0U;
    }

    if (ParseU8_3(&raw[36], &out->load_percent_analog) == 0U)
    {
        return 0U;
    }

    /* bits 19 ตัว: raw[40]..raw[58] */
    for (i = 40U; i <= 58U; i++)
    {
        if ((raw[i] != '0') && (raw[i] != '1'))
        {
            return 0U;
        }
    }

    out->status.inverter_mode          = (uint8_t)(raw[40] - '0');
    out->status.bypass_mode            = (uint8_t)(raw[41] - '0');
    out->status.utility_present        = (uint8_t)(raw[42] - '0');
    out->status.utility_charger_enable = (uint8_t)(raw[43] - '0');
    out->status.solar_charger_enable   = (uint8_t)(raw[44] - '0');
    out->status.saving_mode            = (uint8_t)(raw[45] - '0');
    out->status.battery_exhausted      = (uint8_t)(raw[46] - '0');
    out->status.shutdown_mode          = (uint8_t)(raw[47] - '0');
    out->status.battery_ovp            = (uint8_t)(raw[48] - '0');
    out->status.remote_shutdown        = (uint8_t)(raw[49] - '0');
    out->status.olp_100_115            = (uint8_t)(raw[50] - '0');
    out->status.olp_115_150            = (uint8_t)(raw[51] - '0');
    out->status.olp_over_150           = (uint8_t)(raw[52] - '0');
    out->status.otp                    = (uint8_t)(raw[53] - '0');
    out->status.inv_uvp                = (uint8_t)(raw[54] - '0');
    out->status.inv_ovp                = (uint8_t)(raw[55] - '0');
    out->status.inv_fault              = (uint8_t)(raw[56] - '0');
    out->status.eeprom_error           = (uint8_t)(raw[57] - '0');
    out->status.system_shutdown        = (uint8_t)(raw[58] - '0');

    (void)temp_u16;
    return 1U;
}

uint8_t NTS250_Q_CMD_Request(void)
{
    if (NTS250_id >= UART_COMM_MAX)
    {
        return 0U;
    }

    if (NTS250_state != 0U)
    {
        return 0U;
    }

    if (UART_Comm_IsBusy(NTS250_id) != 0U)
    {
        return 0U;
    }

    Create_Q_CMD(tx_buf);
    UART_Comm_ClearFlags(NTS250_id);
    NTS250_ready = 0U;

    if (UART_Comm_Transmit_IT(NTS250_id, tx_buf, DC_REQ_LEN) == 0U)
    {
        return 0U;
    }

    NTS250_state = 1U;
    return 1U;
}


void NTS250_init(Uart_Comm_id_t uart_id)
{
    NTS250_id = uart_id;
    NTS250_state = 0U;
    NTS250_ready = 0U;

    memset(tx_buf, 0, sizeof(tx_buf));
    memset(rx_buf, 0, sizeof(rx_buf));
    NTS250_Q_CMD_Request();
}

void NTS250_Task(void)
{
    if (NTS250_id >= UART_COMM_MAX)
    {
        return;
    }

    if (UART_Comm_HasError(NTS250_id) != 0U)
    {
        UART_Comm_ClearFlags(NTS250_id);
        NTS250_state = 0U;
        rx_len = 0U;
        return;
    }

    if (NTS250_state == 1U)
    {
        if (UART_Comm_IsTxDone(NTS250_id) != 0U)
        {
            UART_Comm_ClearFlags(NTS250_id);

            rx_len = 0U;
            memset(rx_buf, 0, sizeof(rx_buf));

            if (UART_Comm_Receive_IT(NTS250_id, &rx_byte, 1U) != 0U)
            {
                NTS250_state = 2U;
            }
            else
            {
                NTS250_state = 0U;
            }
        }
    }
    else if (NTS250_state == 2U)
    {
        if (UART_Comm_IsRxDone(NTS250_id) != 0U)
        {
            UART_Comm_ClearFlags(NTS250_id);

            if (rx_len < sizeof(rx_buf))
            {
                rx_buf[rx_len++] = rx_byte;
            }
            else
            {
                // buffer overflow
                NTS250_state = 0U;
                rx_len = 0U;
                return;
            }

            // เจอ CR = จบเฟรม
            if (rx_byte == 0x0DU)
            {
                NTS250_state = 0U;

                if (NTS250_Parse_Q_CMD(rx_buf, rx_len, &q_data) != 0U)
                {
                    q_data_valid = 1U;
                    NTS250_ready = 1U;
                }
                else
                {
                    q_data_valid = 0U;
                    NTS250_ready = 0U;
                }
            }
            else
            {
                // รับต่ออีก 1 ไบต์
                if (UART_Comm_Receive_IT(NTS250_id, &rx_byte, 1U) == 0U)
                {
                    NTS250_state = 0U;
                    rx_len = 0U;
                }
            }
        }
    }
}

uint8_t NTS250_GetQData(InverterQData_t *out)
{
    if ((out == NULL) || (q_data_valid == 0U) || (NTS250_ready == 0U))
    {
        return 0U;
    }

    *out = q_data;
    NTS250_ready = 0U;
    return 1U;
}


