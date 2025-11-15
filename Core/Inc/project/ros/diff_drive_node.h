#ifndef ROS_DIFF_DRIVE_NODE_H
#define ROS_DIFF_DRIVE_NODE_H

#include <stdint.h>

void Ros_DiffDriveNode_Init();
void Ros_DiffDriveNode_SpinExec(uint32_t timeout_ns);

#endif