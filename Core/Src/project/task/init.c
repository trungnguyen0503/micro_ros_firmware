#include "project/actuator/motor.h"
#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/ros/odometry_node.h"
#include "project/ros/teleop_node.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"

#include "cmsis_os2.h"
#include "project/utility.h"
#include "rmw_microros/time_sync.h"

#include <inttypes.h>
#include <math.h>

void StartInitTask(void *argument) {
    UNUSED(argument);
    Utility_Log(Utility_LogInfo, "Initialize task started");

    const uint32_t start_init_ms = osKernelGetTickCount();

    Utility_Log(Utility_LogInfo, "Initializing actuators and sensors");
    Actuator_Motor_Init();
    Sensor_Imu_Init();
    Sensor_MotorEncoder_Init();

    Utility_Log(Utility_LogInfo, "Initializing ros components");
    Ros_Init();

    Utility_Log(Utility_LogInfo, "Initializing ros nodes");
    Ros_ImuNode_Init();
    Ros_OdometryNode_Init();
    Ros_BatteryNode_Init();
    Ros_TeleopNode_Init();

    const uint32_t end_init_ms = osKernelGetTickCount();
    const uint32_t init_time_ms = end_init_ms - start_init_ms;
    Utility_Log(Utility_LogInfo, "Initializing finished, took %" PRIu32 "ms", init_time_ms);

    osThreadExit();
}
