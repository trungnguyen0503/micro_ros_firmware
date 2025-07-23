#include "project/ros/imu_node.h"
#include "project/ros/global.h"

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
