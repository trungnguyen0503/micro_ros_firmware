#ifndef ACTUATOR_MOTOR_H
#define ACTUATOR_MOTOR_H

#include "tim.h"

#include <math.h>

#define Actuator_Motor_LEFT_HTIM htim9
#define Actuator_Motor_RIGHT_HTIM htim2

#define Actuator_Motor_LEFT_FORWARD_CHANNEL TIM_CHANNEL_2
#define Actuator_Motor_LEFT_BACKWARD_CHANNEL TIM_CHANNEL_1
#define Actuator_Motor_RIGHT_FORWARD_CHANNEL TIM_CHANNEL_4
#define Actuator_Motor_RIGHT_BACKWARD_CHANNEL TIM_CHANNEL_3

#define Actuator_Motor_MIN_RPM 26.0
#define Actuator_Motor_MAX_RPM 278.0
#define Actuator_Motor_MIN_ANGULAR_VEL (Actuator_Motor_MIN_RPM * 2 * M_PI / 60)
#define Actuator_Motor_MAX_ANGULAR_VEL (Actuator_Motor_MAX_RPM * 2 * M_PI / 60)

void Actuator_Motor_Init(void);
void Actuator_Motor_SetLeftPwmPercent(double percent);
void Actuator_Motor_SetRightPwmPercent(double percent);
double Actuator_Motor_LeftAngularVelToPwmPercent(double w);
double Actuator_Motor_RightAngularVelToPwmPercent(double w);
void Actuator_Motor_SetLeftAngularVel(double w);
void Actuator_Motor_SetRightAngularVel(double w);

#endif
