#ifndef SENSOR_IMU_H
#define SENSOR_IMU_H

#include "geometry_msgs/msg/vector3.h"

void Sensor_Imu_Init();
geometry_msgs__msg__Vector3 Sensor_Imu_GetAccel();
geometry_msgs__msg__Vector3 Sensor_Imu_GetGyro();
geometry_msgs__msg__Vector3 Sensor_Imu_GetMag();

#endif