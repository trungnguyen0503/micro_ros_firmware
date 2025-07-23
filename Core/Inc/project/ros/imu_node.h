#ifndef ROS_IMU_NODE_H
#define ROS_IMU_NODE_H

#include "rclc/node.h"
#include "rclc/publisher.h"

extern rcl_node_t Ros_Imu_node;
extern rcl_publisher_t Ros_Imu_imu_data_pub;
extern rcl_publisher_t Ros_Imu_magnetometer_data_pub;

void Ros_Imu_InitNode();

#endif
