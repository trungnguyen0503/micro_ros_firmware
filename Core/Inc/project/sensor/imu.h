#ifndef SENSOR_IMU_H
#define SENSOR_IMU_H

#include "geometry_msgs/msg/vector3.h"

#define Sensor_Imu_FRAME "imu_link"

void Sensor_Imu_Init();
// Unit: m/(s^2)
geometry_msgs__msg__Vector3 Sensor_Imu_GetAccel();
// Unit: rad/s
geometry_msgs__msg__Vector3 Sensor_Imu_GetGyro();
// Unit: T
geometry_msgs__msg__Vector3 Sensor_Imu_GetMag();

#endif