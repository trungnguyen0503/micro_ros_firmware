#include "project/sensor/motor_encoder.h"

#include "cmsis_os2.h"

static uint32_t Sensor_MotorEncoder_last_left_read_tick = 0;
static uint32_t Sensor_MotorEncoder_last_right_read_tick = 0;

#define COUNT_ROOT (UINT16_MAX / 2)

void Sensor_MotorEncoder_Init(void) {
    HAL_TIM_Encoder_Start(&Sensor_MotorEncoder_LEFT_HTIM, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&Sensor_MotorEncoder_RIGHT_HTIM, TIM_CHANNEL_ALL);
}

float Sensor_MotorEncoder_GetLeftRevCount() {
    Sensor_MotorEncoder_last_left_read_tick = osKernelGetTickCount();
    const uint32_t raw_count = __HAL_TIM_GET_COUNTER(&Sensor_MotorEncoder_LEFT_HTIM);
    __HAL_TIM_SET_COUNTER(&Sensor_MotorEncoder_LEFT_HTIM, COUNT_ROOT);
    int32_t count = (int32_t)raw_count - COUNT_ROOT;
    if (count < 0) {
        count = -count;
    }
    return (float)count / Sensor_MotorEncoder_CPR;
}

float Sensor_MotorEncoder_GetRightRevCount() {
    Sensor_MotorEncoder_last_right_read_tick = osKernelGetTickCount();
    const uint32_t raw_count = __HAL_TIM_GET_COUNTER(&Sensor_MotorEncoder_RIGHT_HTIM);
    __HAL_TIM_SET_COUNTER(&Sensor_MotorEncoder_RIGHT_HTIM, COUNT_ROOT);
    int32_t count = (int32_t)raw_count - COUNT_ROOT;
    if (count < 0) {
        count = -count;
    }
    return (float)count / Sensor_MotorEncoder_CPR;
}

float Sensor_MotorEncoder_GetLeftRpm() {
    const uint32_t last_tick = Sensor_MotorEncoder_last_left_read_tick;
    const float rev_count = Sensor_MotorEncoder_GetLeftRevCount();
    const uint32_t delta_ms = Sensor_MotorEncoder_last_left_read_tick - last_tick;
    return rev_count / (float)delta_ms * 60000.F;
}

float Sensor_MotorEncoder_GetRightRpm() {
    const uint32_t last_tick = Sensor_MotorEncoder_last_right_read_tick;
    const float rev_count = Sensor_MotorEncoder_GetRightRevCount();
    const uint32_t delta_ms = Sensor_MotorEncoder_last_right_read_tick - last_tick;
    return rev_count / (float)delta_ms * 60000.F;
}
