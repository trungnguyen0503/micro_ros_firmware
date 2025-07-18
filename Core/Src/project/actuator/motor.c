#include "project/actuator/motor.h"

#include <math.h>

void Actuator_Motor_Init(void) {
    HAL_TIM_PWM_Start(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL);
}

void Actuator_Motor_SetLeftPwmPercent(const double percent) {
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL, 0);
    if (percent > 100.0) {
        // TODO: invalid percentage
        return;
    }
    if (percent < -100.0) {
        // TODO: invalid percentage
        return;
    }
    double percent_postive = percent;
    uint32_t channel = Actuator_Motor_LEFT_FORWARD_CHANNEL;
    if (percent < 0.0) {
        percent_postive = -percent;
        channel = Actuator_Motor_LEFT_BACKWARD_CHANNEL;
    }
    const uint32_t autoreload = __HAL_TIM_GET_AUTORELOAD(&Actuator_Motor_LEFT_HTIM);
    const uint16_t compare = (uint16_t)((double)autoreload / 100.0 * percent_postive);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, channel, compare);
}

void Actuator_Motor_SetRightPwmPercent(const double percent) {
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL, 0);
    if (percent > 100.0) {
        // TODO: invalid percentage
        return;
    }
    if (percent < -100.0) {
        // TODO: invalid percentage
        return;
    }
    double percent_postive = percent;
    uint32_t channel = Actuator_Motor_RIGHT_FORWARD_CHANNEL;
    if (percent < 0.0) {
        percent_postive = -percent;
        channel = Actuator_Motor_RIGHT_BACKWARD_CHANNEL;
    }
    const uint32_t autoreload = __HAL_TIM_GET_AUTORELOAD(&Actuator_Motor_RIGHT_HTIM);
    const uint16_t compare = (uint16_t)((double)autoreload / 100.0 * percent_postive);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, channel, compare);
}

double Actuator_Motor_LeftAngularVelToPwmPercent(const double w) {
    const double rpm = w * 60.0 / 2.0 / M_PI;
    const double abs_rpm = fabs(rpm);
    if (abs_rpm < Actuator_Motor_MIN_RPM || abs_rpm > Actuator_Motor_MAX_RPM) {
        return 0.0;
    }
    if (rpm >= 0) {
        return 52.5 + 0.146 * rpm + 3.76E-5 * rpm * rpm;
    }
    // TODO:
    return 0.0;
}

double Actuator_Motor_RightAngularVelToPwmPercent(const double w) {
    const double rpm = w * 60.0 / 2.0 / M_PI;
    const double abs_rpm = fabs(rpm);
    if (abs_rpm < Actuator_Motor_MIN_RPM || abs_rpm > Actuator_Motor_MAX_RPM) {
        return 0.0;
    }
    if (rpm >= 0) {
        return 52.0 + 0.174 * rpm - 1.44E-5 * rpm * rpm;
    }
    // TODO:
    return 0.0;
}

void Actuator_Motor_SetLeftAngularVel(const double w) {
    const double pwm_percent = Actuator_Motor_LeftAngularVelToPwmPercent(w);
    Actuator_Motor_SetLeftPwmPercent(pwm_percent);
}

void Actuator_Motor_SetRightAngularVel(const double w) {
    const double pwm_percent = Actuator_Motor_RightAngularVelToPwmPercent(w);
    Actuator_Motor_SetRightPwmPercent(pwm_percent);
}
