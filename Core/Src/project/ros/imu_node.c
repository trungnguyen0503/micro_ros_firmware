#include "project/ros/imu_node.h"
#include "project/ros/global.h"
#include "project/sensor/imu.h"
#include "project/utility.h"

#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

static rcl_node_t Ros_ImuNode_handle = { 0 };
static rcl_publisher_t Ros_ImuNode_imu_data_pub = { 0 };
static rcl_publisher_t Ros_ImuNode_mag_data_pub = { 0 };

void Ros_ImuNode_Init() {
    rclc_node_init_default(&Ros_ImuNode_handle, "imu_node", "", Ros_GetSupportStruct());
    rclc_publisher_init_best_effort(
        &Ros_ImuNode_imu_data_pub,
        &Ros_ImuNode_handle,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        "imu/data"
    );
    rclc_publisher_init_best_effort(
        &Ros_ImuNode_mag_data_pub,
        &Ros_ImuNode_handle,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField),
        "magnetic_field/data"
    );
}

const rcl_node_t *Ros_ImuNode_GetHandle() {
    return &Ros_ImuNode_handle;
}

const rcl_publisher_t *Ros_ImuNode_GetImuDataPub() {
    return &Ros_ImuNode_imu_data_pub;
}

const rcl_publisher_t *Ros_ImuNode_GetMagDataPub() {
    return &Ros_ImuNode_mag_data_pub;
}

void Ros_ImuNode_PublishImuData() {
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

void Ros_ImuNode_PublishMagData() {
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
