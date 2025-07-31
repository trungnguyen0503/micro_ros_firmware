#ifndef ROS_BATTERY_NODE_H
#define ROS_BATTERY_NODE_H

#include "rclc/node.h"
#include "rclc/publisher.h"

void Ros_BatteryNode_Init();
const rcl_node_t *Ros_BatteryNode_GetHandle();
const rcl_publisher_t *Ros_BatteryNode_GetBatteryDataPub();
void Ros_BatteryNode_PublishBatteryData();

#endif