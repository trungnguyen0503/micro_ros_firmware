#include "project/sensor/motor_encoder.h"

#include "cmsis_os2.h"
#include "math.h"

static uint32_t g_last_left_read_tick = 0;
static uint32_t g_last_right_read_tick = 0;

#define COUNT_ROOT (UINT16_MAX / 2)

void Sensor_MotorEncoder_Init(void) {
    HAL_TIM_Encoder_Start(&Sensor_MotorEncoder_LEFT_HTIM, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&Sensor_MotorEncoder_RIGHT_HTIM, TIM_CHANNEL_ALL);
}

double Sensor_MotorEncoder_GetLeftRevCount() {
    g_last_left_read_tick = osKernelGetTickCount();
    const uint32_t raw_count = __HAL_TIM_GET_COUNTER(&Sensor_MotorEncoder_LEFT_HTIM);
    __HAL_TIM_SET_COUNTER(&Sensor_MotorEncoder_LEFT_HTIM, COUNT_ROOT);
    const int32_t count = COUNT_ROOT - (int32_t)raw_count;
    return (double)count / Sensor_MotorEncoder_CPR;
}

double Sensor_MotorEncoder_GetRightRevCount() {
    g_last_right_read_tick = osKernelGetTickCount();
    const uint32_t raw_count = __HAL_TIM_GET_COUNTER(&Sensor_MotorEncoder_RIGHT_HTIM);
    __HAL_TIM_SET_COUNTER(&Sensor_MotorEncoder_RIGHT_HTIM, COUNT_ROOT);
    const int32_t count = (int32_t)raw_count - COUNT_ROOT;
    return (double)count / Sensor_MotorEncoder_CPR;
}

double Sensor_MotorEncoder_GetLeftAngularVel() {
    const uint32_t last_tick = g_last_left_read_tick;
    const double rev_count = Sensor_MotorEncoder_GetLeftRevCount();
    const uint32_t delta_ms = g_last_left_read_tick - last_tick;
    return rev_count / delta_ms * 2000.0 * M_PI;
}

double Sensor_MotorEncoder_GetRightAngularVel() {
    const uint32_t last_tick = g_last_right_read_tick;
    const double rev_count = Sensor_MotorEncoder_GetRightRevCount();
    const uint32_t delta_ms = g_last_right_read_tick - last_tick;
    return rev_count / delta_ms * 2000.0 * M_PI;
}
