#ifndef SENSOR_MOTOR_ENCODER_H
#define SENSOR_MOTOR_ENCODER_H

#include "tim.h"

#define Sensor_MotorEncoder_CPR 900

#define Sensor_MotorEncoder_LEFT_HTIM htim3
#define Sensor_MotorEncoder_RIGHT_HTIM htim8

void Sensor_MotorEncoder_Init(void);
// Unit: revolution
double Sensor_MotorEncoder_GetLeftRevCount();
// Unit: revolution
double Sensor_MotorEncoder_GetRightRevCount();
// Unit: rad/s
double Sensor_MotorEncoder_GetLeftAngularVel();
// Unit: rad/s
double Sensor_MotorEncoder_GetRightAngularVel();

#endif
