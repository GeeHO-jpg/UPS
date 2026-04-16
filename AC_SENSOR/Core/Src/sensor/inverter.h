/*
 * Inverter.h
 *
 *  Created on: Mar 5, 2026
 *      Author: _TTTXN
 */

#ifndef INC_INVERTER_H_
#define INC_INVERTER_H_


#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int   VVV;
    int   QQQ;
    float SS_S;
    int   BBB;
    float TT_T;
    int   MMM;
    float RR_R;
    int   DDD;
    int   PPP;
    bool  b[19];
    bool  valid;
} NTS250P212_QStatus;



static inline bool NTS250P212_ParseIntField(const char* frame, int pos, int width, int* value) {
    char temp[8] = {0};
    char* end = NULL;
    long parsed;

    if ((frame == NULL) || (value == NULL)) {
        return false;
    }

    memcpy(temp, &frame[pos], (size_t)width);
    parsed = strtol(temp, &end, 10);
    if (end == temp) {
        return false;
    }

    *value = (int)parsed;
    return true;
}

static inline bool NTS250P212_ParseFloatField(const char* frame, int pos, int width, float* value) {
    char temp[8] = {0};
    char* end = NULL;
    float parsed;

    if ((frame == NULL) || (value == NULL)) {
        return false;
    }

    memcpy(temp, &frame[pos], (size_t)width);
    parsed = strtof(temp, &end);
    if (end == temp) {
        return false;
    }

    *value = parsed;
    return true;
}

static inline const uint8_t* NTS250P212_RequestQ(UART_HandleTypeDef *Port) {

    const uint8_t cmdQ[2] = { 'Q', '\r' };
    static uint8_t response[512];  // Static buffer to store response
    
    if (Port == NULL) {
        return NULL;
    }

    

    if (HAL_UART_Transmit(Port, (uint8_t*)cmdQ, 2, 100) != HAL_OK) {
        return NULL;
    }

    if (HAL_UART_Receive(Port, response, sizeof(response) - 1U, 200) != HAL_OK) {
        return NULL;
    }

    response[sizeof(response) - 1U] = '\0';
    return response;
}

static inline NTS250P212_QStatus NTS250P212_GetData(const char* frame) {
    const int sepPos[] = {4, 8, 13, 17, 22, 26, 31, 35, 39};
    NTS250P212_QStatus parsed = {0};
    size_t i;
    size_t len;

    if (frame == NULL) {
        return parsed;
    }

    len = strnlen(frame, 128);
    if (len < 60U) {
        return parsed;
    }

    if ((frame[0] != '(') || (frame[59] != ')')) {
        return parsed;
    }

    for (i = 0; i < (sizeof(sepPos) / sizeof(sepPos[0])); ++i) {
        if (frame[sepPos[i]] != ' ') {
            return parsed;
        }
    }

    if (!NTS250P212_ParseIntField(frame, 1, 3, &parsed.VVV)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseIntField(frame, 5, 3, &parsed.QQQ)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseFloatField(frame, 9, 4, &parsed.SS_S)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseIntField(frame, 14, 3, &parsed.BBB)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseFloatField(frame, 18, 4, &parsed.TT_T)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseIntField(frame, 23, 3, &parsed.MMM)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseFloatField(frame, 27, 4, &parsed.RR_R)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseIntField(frame, 32, 3, &parsed.DDD)) return (NTS250P212_QStatus){0};
    if (!NTS250P212_ParseIntField(frame, 36, 3, &parsed.PPP)) return (NTS250P212_QStatus){0};

    for (i = 0; i < 19U; ++i) {
        char c = frame[40 + (int)i];
        if ((c != '0') && (c != '1')) {
            return (NTS250P212_QStatus){0};
        }
        parsed.b[i] = (c == '1');
    }

    parsed.valid = true;
    return parsed;
}

static inline bool NTS250P212_InverterMode(const NTS250P212_QStatus* s) {
    return (s != NULL) ? s->b[0] : false;
}

static inline bool NTS250P212_BypassMode(const NTS250P212_QStatus* s) {
    return (s != NULL) ? s->b[1] : false;
}

static inline bool NTS250P212_SystemShutdown(const NTS250P212_QStatus* s) {
    return (s != NULL) ? s->b[18] : false;
}

#ifdef __cplusplus
}   
#endif

#endif /* INC_INVERTER_H_ */
