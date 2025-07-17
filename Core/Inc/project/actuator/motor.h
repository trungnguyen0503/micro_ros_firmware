#ifndef ACTUATOR_MOTOR_H
#define ACTUATOR_MOTOR_H

#include "tim.h"

#define Actuator_Motor_LEFT_HTIM htim9
#define Actuator_Motor_RIGHT_HTIM htim2

#define Actuator_Motor_LEFT_FORWARD_CHANNEL TIM_CHANNEL_2
#define Actuator_Motor_LEFT_BACKWARD_CHANNEL TIM_CHANNEL_1
#define Actuator_Motor_RIGHT_FORWARD_CHANNEL TIM_CHANNEL_4
#define Actuator_Motor_RIGHT_BACKWARD_CHANNEL TIM_CHANNEL_3

void Actuator_Motor_Init(void);
void Actuator_Motor_SetLeftPwmPercent(float percent);
void Actuator_Motor_SetRightPwmPercent(float percent);

#endif
