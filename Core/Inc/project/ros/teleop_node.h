#ifndef ROS_TELEOP_NODE_H
#define ROS_TELEOP_NODE_H

#include "geometry_msgs/msg/twist.h"
#include "rcl/subscription.h"
#include "rclc/executor.h"
#include "rclc/node.h"
#include "rclc/subscription.h"

void Ros_TeleopNode_Init();
const rcl_node_t *Ros_TeleopNode_GetHandle();
const rcl_subscription_t *Ros_TeleopNode_GetTeleopDataSub();
rclc_executor_t* Ros_TeleopNode_GetExec();
void Ros_TeleopNode_Callback(const void *void_msg);

#endif
