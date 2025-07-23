#ifndef ROS_BATTERY_NODE_H
#define ROS_BATTERY_NODE_H

#include "rclc/node.h"
#include "rclc/publisher.h"

extern rcl_node_t Ros_Battery_node;
extern rcl_publisher_t Ros_Battery_data_pub;

void Ros_Battery_InitNode();

#endif