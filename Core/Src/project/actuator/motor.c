#include "project/actuator/motor.h"

#include <math.h>

void Actuator_Motor_Init(void) {
    HAL_TIM_PWM_Start(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL);
    HAL_TIM_PWM_Start(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL);
}

void Actuator_Motor_SetLeftPwmPercent(double percent) {
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL, 0);
    const uint32_t autoreload = __HAL_TIM_GET_AUTORELOAD(&Actuator_Motor_LEFT_HTIM);
    if (percent == 0) {
        __HAL_TIM_SET_COMPARE(
            &Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_BACKWARD_CHANNEL, autoreload
        );
        __HAL_TIM_SET_COMPARE(
            &Actuator_Motor_LEFT_HTIM, Actuator_Motor_LEFT_FORWARD_CHANNEL, autoreload
        );
        return;
    }
    if (percent > 100.0) {
        percent = 100.0;
    }
    if (percent < -100.0) {
        percent = -100.0;
    }
    double abs_percent = percent;
    uint32_t channel = Actuator_Motor_LEFT_FORWARD_CHANNEL;
    if (percent < 0.0) {
        abs_percent = -percent;
        channel = Actuator_Motor_LEFT_BACKWARD_CHANNEL;
    }
    const uint16_t compare = (uint16_t)((double)autoreload / 100.0 * abs_percent);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_LEFT_HTIM, channel, compare);
}

void Actuator_Motor_SetRightPwmPercent(double percent) {
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL, 0);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL, 0);
    const uint32_t autoreload = __HAL_TIM_GET_AUTORELOAD(&Actuator_Motor_RIGHT_HTIM);
    if (percent == 0) {
        __HAL_TIM_SET_COMPARE(
            &Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_BACKWARD_CHANNEL, autoreload
        );
        __HAL_TIM_SET_COMPARE(
            &Actuator_Motor_RIGHT_HTIM, Actuator_Motor_RIGHT_FORWARD_CHANNEL, autoreload
        );
        return;
    }
    if (percent > 100.0) {
        percent = 100.0;
    }
    if (percent < -100.0) {
        percent = -100.0;
    }
    double abs_percent = percent;
    uint32_t channel = Actuator_Motor_RIGHT_FORWARD_CHANNEL;
    if (percent < 0.0) {
        abs_percent = -percent;
        channel = Actuator_Motor_RIGHT_BACKWARD_CHANNEL;
    }
    const uint16_t compare = (uint16_t)((double)autoreload / 100.0 * abs_percent);
    __HAL_TIM_SET_COMPARE(&Actuator_Motor_RIGHT_HTIM, channel, compare);
}

double Actuator_Motor_LeftAngularVelToPwmPercent(const double w) {
    const double rpm = w * 60.0 / 2.0 / M_PI;
    double abs_rpm = fabs(rpm);
    if (abs_rpm < Actuator_Motor_MIN_RPM) {
        return 0.0;
    }
    if (abs_rpm > Actuator_Motor_MAX_RPM) {
        abs_rpm = Actuator_Motor_MAX_RPM;
    }
    if (rpm >= 0) {
        // return 52.5 + (0.146 * abs_rpm) + (3.76E-5 * abs_rpm * abs_rpm);
    return 52.0 + (0.31 * abs_rpm) + (3.56E-4 * abs_rpm * abs_rpm);
    }
    // return -52.0 - (0.159 * abs_rpm) - (3.38E-5 * abs_rpm * abs_rpm);
    return -51.5 - (0.3346 * abs_rpm) - (3.8E-4 * abs_rpm * abs_rpm);
}

double Actuator_Motor_RightAngularVelToPwmPercent(const double w) {
    const double rpm = w * 60.0 / 2.0 / M_PI;
    double abs_rpm = fabs(rpm);
    if (abs_rpm < Actuator_Motor_MIN_RPM) {
        return 0.0;
    }
    if (abs_rpm > Actuator_Motor_MAX_RPM) {
        abs_rpm = Actuator_Motor_MAX_RPM;
    }
    if (rpm >= 0) {
        // return 52.0 + (0.174 * abs_rpm) - (1.44E-5 * abs_rpm * abs_rpm);
        return 52.6 + (0.351 * abs_rpm) + (12.7E-5 * abs_rpm * abs_rpm);
    }
    // return -52.7 - (0.152 * abs_rpm) - (1.08E-5 * abs_rpm * abs_rpm);
    return - 53.0 - (0.32 * abs_rpm) - (3.2E-4 * abs_rpm * abs_rpm);
}

void Actuator_Motor_SetLeftAngularVel(const double w) {
    const double pwm_percent = Actuator_Motor_LeftAngularVelToPwmPercent(w);
    Actuator_Motor_SetLeftPwmPercent(pwm_percent);
}

void Actuator_Motor_SetRightAngularVel(const double w) {
    const double pwm_percent = Actuator_Motor_RightAngularVelToPwmPercent(w);
    Actuator_Motor_SetRightPwmPercent(pwm_percent);
}
