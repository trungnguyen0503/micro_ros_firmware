#ifndef SENSOR_MOTOR_ENCODER_H
#define SENSOR_MOTOR_ENCODER_H

#include "tim.h"

#define Sensor_MotorEncoder_CPR 900

#define Sensor_MotorEncoder_LEFT_HTIM htim3
#define Sensor_MotorEncoder_RIGHT_HTIM htim8

void Sensor_MotorEncoder_Init(void);
float Sensor_MotorEncoder_GetLeftRevCount();
float Sensor_MotorEncoder_GetRightRevCount();
float Sensor_MotorEncoder_GetLeftRpm();
float Sensor_MotorEncoder_GetRightRpm();

#endif