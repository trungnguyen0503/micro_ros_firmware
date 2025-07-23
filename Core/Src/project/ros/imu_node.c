#include "project/ros/imu_node.h"
#include "project/ros/global.h"

#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

rcl_node_t Ros_Imu_node = { 0 };
rcl_publisher_t Ros_Imu_imu_data_pub = { 0 };
rcl_publisher_t Ros_Imu_magnetometer_data_pub = { 0 };

void Ros_Imu_InitNode() {
    rclc_node_init_default(&Ros_Imu_node, "imu_node", "", &Ros_support);
    rclc_publisher_init_best_effort(
        &Ros_Imu_imu_data_pub,
        &Ros_Imu_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        "imu/data"
    );
    rclc_publisher_init_best_effort(
        &Ros_Imu_magnetometer_data_pub,
        &Ros_Imu_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField),
        "magnetometer/data"
    );
}
