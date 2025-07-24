#include "project/actuator/motor.h"
#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/ros/teleop_node.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"

#include "cmsis_os2.h"
#include "rmw_microros/time_sync.h"

#include <math.h>

void StartInitTask(void *argument) {
    UNUSED(argument);

    Actuator_Motor_Init();
    Sensor_Imu_Init();
    Sensor_MotorEncoder_Init();

    Ros_Init();

    // Sync time with agent using NTP
    {
        const rmw_ret_t ret = rmw_uros_sync_session(1000);
        if (ret != RMW_RET_OK) {
            // TODO:
        }
    }

    Ros_ImuNode_Init();
    Ros_BatteryNode_Init();
    Ros_TeleopNode_Init();

    osThreadExit();
}
