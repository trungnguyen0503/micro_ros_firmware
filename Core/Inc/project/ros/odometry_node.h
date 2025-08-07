#ifndef ROS_ODOMETRY_NODE_H
#define ROS_ODOMETRY_NODE_H

#include "rclc/executor.h"
#include "rclc/node.h"
#include "rclc/publisher.h"
#include "rclc/subscription.h"

#define Ros_OdometryNode_ODOMETRY_DATA_TOPIC "odometry/data"

void Ros_OdometryNode_Init();
void Ros_OdometryNode_SpinExec(uint32_t timeout_ns);
void Ros_OdometryNode_PublishOdoData();

#endif
