#include "project/actuator/motor.h"
#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/ros/odometry_node.h"
#include "project/ros/teleop_node.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"
#include "project/utility.h"

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

#include <inttypes.h>
#include <math.h>

extern osThreadId_t InitTaskHandle;

static bool g_task_is_done = false;

bool Task_Init_IsDone() {
    return g_task_is_done;
}

void Task_Init_WaitUntilDone() {
    while (!g_task_is_done) {
        osDelay(100);
    }
}

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

    Utility_Log(Utility_LogInfo, "Rotating robot to calibrate magnetometer");
    Actuator_Motor_SetLeftAngularVel(-Actuator_Motor_MAX_ANGULAR_VEL / 3);
    Actuator_Motor_SetRightAngularVel(Actuator_Motor_MAX_ANGULAR_VEL / 3);
    for (int i = 0; i < 75; i++) {
        Sensor_Imu_GetMag();
        osDelay(20);
    }
    Actuator_Motor_SetLeftAngularVel(Actuator_Motor_MAX_ANGULAR_VEL / 3);
    Actuator_Motor_SetRightAngularVel(-Actuator_Motor_MAX_ANGULAR_VEL / 3);
    for (int i = 0; i < 75; i++) {
        Sensor_Imu_GetMag();
        osDelay(20);
    }
    Actuator_Motor_SetLeftAngularVel(0);
    Actuator_Motor_SetRightAngularVel(0);
    osDelay(500);
    Ros_OdometryNode_RecordInitialHeading();

    const uint32_t stack_left = uxTaskGetStackHighWaterMark(InitTaskHandle);
    const uint32_t end_init_ms = osKernelGetTickCount();
    const uint32_t init_time_ms = end_init_ms - start_init_ms;
    Utility_Log(
        Utility_LogInfo, "Initializing finished, took %" PRIu32 "ms. Memory left: %" PRIu32 "B",
        init_time_ms, stack_left * 4
    );

    g_task_is_done = true;
    osThreadExit();
}
