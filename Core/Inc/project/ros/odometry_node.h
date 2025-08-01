#ifndef ROS_ODOMETRY_NODE_H
#define ROS_ODOMETRY_NODE_H

#include "rclc/executor.h"
#include "rclc/node.h"
#include "rclc/publisher.h"
#include "rclc/subscription.h"

void Ros_OdometryNode_Init();
void Ros_OdometryNode_RecordInitialHeading();
const rcl_node_t *Ros_OdometryNode_GetHandle();
const rcl_subscription_t *Ros_OdometryNode_GetImuDataSub();
const rcl_subscription_t *Ros_OdometryNode_GetMagDataSub();
const rcl_publisher_t *Ros_OdometryNode_GetOdoDataPub();
rclc_executor_t *Ros_OdometryNode_GetExec();
void Ros_OdometryNode_ImuDataCallback(const void *void_msg);
void Ros_OdometryNode_MagDataCallback(const void *void_msg);
void Ros_OdometryNode_PublishOdoData();

#endif
