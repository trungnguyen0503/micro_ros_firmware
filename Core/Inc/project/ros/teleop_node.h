#ifndef ROS_TELEOP_NODE_H
#define ROS_TELEOP_NODE_H

#include "geometry_msgs/msg/twist.h"
#include "rcl/rcl.h"
#include "rclc/executor.h"

extern rcl_node_t Ros_Teleop_node;
extern rcl_subscription_t Ros_Teleop_data_sub;
extern rclc_executor_t Ros_Teleop_exec;
extern geometry_msgs__msg__Twist Ros_Teleop_msg;

void Ros_Teleop_callback(const void *void_msg);

#endif