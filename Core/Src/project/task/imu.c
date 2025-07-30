#include "project/sensor/imu.h"
#include "project/ros/imu_node.h"
#include "project/task/init.h"
#include "project/utility.h"

#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#include "cmsis_os2.h"

#include <math.h>

void StartIMUTask(void *argument) {
    (void)argument;
    Task_Init_WaitUntilDone();

    while (1) {
        {
            const sensor_msgs__msg__Imu msg = {
                .header.frame_id = Utility_MakeStaticRosCString("base_link"),
                .header.stamp = Utility_GetRosTimeStamp(),
                .angular_velocity = Sensor_Imu_GetGyro(),
                .linear_acceleration = Sensor_Imu_GetAccel(),
                .orientation.w = 1,
                .angular_velocity_covariance[0] = -1,
                .linear_acceleration_covariance[0] = -1,
                .orientation_covariance[0] = -1,
            };
            const rcl_ret_t ret = rcl_publish(Ros_ImuNode_GetImuDataPub(), &msg, NULL);
            if (ret != RCL_RET_OK) {
                Utility_Log(Utility_LogWarning, "/imu/data publish failed (code %d)", ret);
            }
        }
        {
            const sensor_msgs__msg__MagneticField msg = {
                .header.frame_id = Utility_MakeStaticRosCString("base_link"),
                .header.stamp = Utility_GetRosTimeStamp(),
                .magnetic_field = Sensor_Imu_GetMag(),
                .magnetic_field_covariance[0] = -1,
            };
            const rcl_ret_t ret = rcl_publish(Ros_ImuNode_GetMagDataPub(), &msg, NULL);
            if (ret != RCL_RET_OK) {
                Utility_Log(
                    Utility_LogWarning, "/magnetic_field/data publish failed (code %d)", ret
                );
            }
        }
        osDelay(50);
    }
}
