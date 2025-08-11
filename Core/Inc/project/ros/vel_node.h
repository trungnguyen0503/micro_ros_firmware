#ifndef ROS_VEL_NODE_H
#define ROS_VEL_NODE_H

#include <stdint.h>

#define Ros_VelNode_VEL_DATA_TOPIC "vel_encoder/data"

void Ros_VelNode_Init();
void Ros_VelNode_PublishVel();

#endif
