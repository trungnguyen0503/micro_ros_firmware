#include "project/actuator/motor.h"
#include "stm32f4xx_hal_tim.h"

void Actuator_Motor_Init(void) {
    HAL_TIM_PWM_Start(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL);
}

void Actuator_Motor_SetLeftPwmPercent(const float percent) {
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL, 0);
    if (percent > 100.F) {
        // TODO: invalid percentage
        return;
    }
    if (percent < -100.F) {
        // TODO: invalid percentage
        return;
    }
    float percent_postive = percent;
    uint32_t channel = Actuator_Motor_LEFT_FORWARD_CHANNEL;
    if (percent < 0.F) {
        percent_postive = -percent;
        channel = Actuator_Motor_LEFT_BACKWARD_CHANNEL;
    }
    const uint32_t autoreload = __HAL_TIM_GET_AUTORELOAD(&Actuator_Motor_LEFT_HTIM);
    const uint16_t compare = (uint16_t)((float)autoreload / 100.F * percent_postive);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, channel, compare);
}

void Actuator_Motor_SetRightPwmPercent(const float percent) {
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL, 0);
    if (percent > 100.F) {
        // TODO: invalid percentage
        return;
    }
    if (percent < -100.F) {
        // TODO: invalid percentage
        return;
    }
    float percent_postive = percent;
    uint32_t channel = Actuator_Motor_RIGHT_FORWARD_CHANNEL;
    if (percent < 0.F) {
        percent_postive = -percent;
        channel = Actuator_Motor_RIGHT_BACKWARD_CHANNEL;
    }
    const uint32_t autoreload = __HAL_TIM_GET_AUTORELOAD(&Actuator_Motor_RIGHT_HTIM);
    const uint16_t compare = (uint16_t)((float)autoreload / 100.F * percent_postive);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, channel, compare);
}
