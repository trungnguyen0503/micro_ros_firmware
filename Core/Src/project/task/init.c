#include "project/actuator/motor.h"
#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/ros/teleop_node.h"
#include "project/sensor/motor_encoder.h"
#include "project/uros/utility.h"

#include "rmw_microros/rmw_microros.h"

#include "cmsis_os2.h"
#include "usart.h"

void StartInitTask(void *argument) {
    UNUSED(argument);

    Actuator_Motor_Init();
    Sensor_MotorEncoder_Init();

    Ros_Init();
    Ros_Imu_InitNode();
    Ros_Battery_InitNode();
    Ros_Teleop_InitNode();

    osThreadExit();
}
