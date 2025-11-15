#include "project/sensor/battery.h"

#include "adc.h"
#include "stm32f446xx.h"

enum {
    ADC_VALUE_MAX = 4095
};

#define ADC_VREF 3.3
#define ADC_SCALE 10.0
#define ADC_OFFSET 0.25

static double g_prev_voltage = 0;

double Sensor_Battery_GetVoltage(const uint32_t timeout_ms) {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, timeout_ms);
    const uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    g_prev_voltage =
        ((double)adc_value / (double)ADC_VALUE_MAX * ADC_VREF * ADC_SCALE) +
        ADC_OFFSET;
    return g_prev_voltage;
}

double Sensor_Battery_GetPrevVoltage() {
    return g_prev_voltage;
}
