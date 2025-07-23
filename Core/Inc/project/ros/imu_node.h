#ifndef ROS_IMU_NODE_H
#define ROS_IMU_NODE_H

#include "rclc/node.h"
#include "rclc/publisher.h"

void Ros_ImuNode_Init();
const rcl_node_t *Ros_ImuNode_GetHandle();
const rcl_publisher_t *Ros_ImuNode_GetImuDataPub();
const rcl_publisher_t *Ros_ImuNode_GetMagDataPub();

#endif
