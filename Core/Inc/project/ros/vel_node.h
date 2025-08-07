#ifndef ROS_VEL_NODE_H
#define ROS_VEL_NODE_H

#include <stdint.h>

#define Ros_VelNode_VEL_DATA_TOPIC "vel/data"

void Ros_VelNode_Init();
void Ros_VelNode_RecordInitialHeading();
void Ros_VelNode_SpinExec(uint32_t timeout_ns);
void Ros_VelNode_PublishVel();

#endif
