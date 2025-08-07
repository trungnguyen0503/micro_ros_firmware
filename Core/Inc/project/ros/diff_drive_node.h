#ifndef ROS_DIFF_DRIVE_NODE_H
#define ROS_DIFF_DRIVE_NODE_H

#include "rclc/executor.h"
#include "rclc/node.h"
#include "rclc/subscription.h"

void Ros_DiffDriveNode_Init();
void Ros_DiffDriveNode_CmdVelLeftCallback(const void *void_msg);
void Ros_DiffDriveNode_CmdVelRightCallback(const void *void_msg);

const rcl_node_t *Ros_DiffDriveNode_GetHandle();
const rcl_subscription_t *Ros_DiffDriveNode_GetCmdVelLeftSub();
const rcl_subscription_t *Ros_DiffDriveNode_GetCmdVelRightSub();

rclc_executor_t *Ros_DiffDriveNode_GetExec();

#endif