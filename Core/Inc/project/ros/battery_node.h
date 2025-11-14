#ifndef ROS_BATTERY_NODE_H
#define ROS_BATTERY_NODE_H

#include "rclc/node.h"
#include "rclc/publisher.h"

void Ros_BatteryNode_Init();
void Ros_BatteryNode_PublishBatteryData();

#endif
