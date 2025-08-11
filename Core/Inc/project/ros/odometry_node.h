#ifndef ROS_ODOMETRY_NODE_H
#define ROS_ODOMETRY_NODE_H

#include <stdint.h>

#define Ros_OdometryNode_ODOMETRY_DATA_TOPIC "odometry/data"

void Ros_OdometryNode_Init();
void Ros_OdometryNode_RecordInitialHeading();
void Ros_OdometryNode_SpinExec(uint32_t timeout_ns);

#endif
