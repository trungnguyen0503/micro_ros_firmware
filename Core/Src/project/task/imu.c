#include "project/sensor/imu.h"
#include "project/icm20948.h"
#include "project/ros/imu_node.h"

#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#include "cmsis_os2.h"

#include <math.h>

void StartIMUTask(void *argument) {
    UNUSED(argument);

    while (1) {
        {
            const uint32_t ms = osKernelGetTickCount();
            sensor_msgs__msg__Imu msg = {
                .header.stamp.sec = (int32_t)(ms / 1000),
                .header.stamp.nanosec = (ms % 1000) * 1000000,
                .angular_velocity = Sensor_Imu_GetGyro(),
                .linear_acceleration = Sensor_Imu_GetAccel(),
                .orientation.w = 1,
                .angular_velocity_covariance[0] = -1,
                .linear_acceleration_covariance[0] = -1,
                .orientation_covariance[0] = -1,
            };
            const rcl_ret_t ret = rcl_publish(Ros_ImuNode_GetImuDataPub(), &msg, NULL);
            if (ret != RCL_RET_OK) {
            }
        }
        {
            const uint32_t ms = osKernelGetTickCount();
            sensor_msgs__msg__MagneticField msg = {
                .header.stamp.sec = (int32_t)(ms / 1000),
                .header.stamp.nanosec = (ms % 1000) * 1000000,
                .magnetic_field = Sensor_Imu_GetMag(),
                .magnetic_field_covariance[0] = -1,
            };
            const rcl_ret_t ret = rcl_publish(Ros_ImuNode_GetMagDataPub(), &msg, NULL);
            if (ret != RCL_RET_OK) {
            }
        }
        osDelay(50);
    }
}
