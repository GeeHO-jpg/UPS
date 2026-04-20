/*
 * AC_sensoe.c
 *
 *  Created on: Apr 6, 2026
 *      Author: _TTTXN
 */

#include "AC_sensor.h"
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include <math.h>

#define VREF            3.3f
#define ADC_MAX_COUNTS  4095.0f
#define PI_F 3.1415927f

static uint16_t period_ms = 0U;
static float ac_scale = 1.0f;     // ค่าคูณจากแรงดันที่ ADC ไปเป็นแรงดันจริง
static float last_vrms = 0.0f;

static void ac_setPeriod(uint8_t f)
{
    if (f == 0U) {
        period_ms = 1U;
        return;
    }
    period_ms = 1000U / f;
}

static float low_pass_filter(float input, float dt, float fc)
{
    float rc;
    float alpha;
    static uint8_t lpf_init = 0;
    static float filtered_vrms;

    if (fc <= 0.0f) {
        return input;
    }

    rc = 1.0f / (2.0f * PI_F * fc);
    alpha = dt / (rc + dt);

    if (lpf_init == 0U)
    {
        filtered_vrms = input;
        lpf_init = 1U;
    }
    else
    {
        filtered_vrms = filtered_vrms + alpha * (input - filtered_vrms);
    }

    return filtered_vrms;
}

static int16_t get_offset(uint16_t raw_adc_data)
{
    static uint32_t current = 0U;
    static uint64_t sum_value = 0U;
    static uint32_t count = 0U;
    static int16_t offset = 0;
    uint32_t now = HAL_GetTick();

    sum_value += raw_adc_data;
    count++;

    if (((now - current) >= period_ms) && (count > 0U))
    {
        current = now;
        offset = (int16_t)(sum_value / count);

        sum_value = 0U;
        count = 0U;
    }

    return offset;
}

uint8_t AC_Get_Voltage(uint16_t raw_adc_data)
{
    static uint64_t sum_sq = 0U;
    static uint32_t count = 0U;
    static uint32_t current = 0U;

    uint32_t now = HAL_GetTick();
    int16_t offset = get_offset(raw_adc_data);
    int32_t pure_raw = (int32_t)raw_adc_data - (int32_t)offset;

    sum_sq += (uint64_t)((int64_t)pure_raw * (int64_t)pure_raw);
    count++;

    if (((now - current) >= 100) && (count > 0U))
    {
        float avg_sq;
        float rms_counts;
        float vrms_adc;

        current = now;

        avg_sq = (float)sum_sq / (float)count;
        rms_counts = sqrtf(avg_sq);

        // Vrms ที่ขา ADC
        vrms_adc = rms_counts * VREF / ADC_MAX_COUNTS;

        // Vrms จริงของต้นทาง หลังคูณ scale วงจร
        last_vrms = vrms_adc * ac_scale;
        last_vrms = low_pass_filter(last_vrms,0.02f,0.5f);

        sum_sq = 0U;
        count = 0U;
    }

    return (uint8_t)last_vrms;
}

void AC_init(uint8_t frequency, float scale)
{
    ac_setPeriod(frequency);
    ac_scale = scale;
    last_vrms = 0.0f;
}
