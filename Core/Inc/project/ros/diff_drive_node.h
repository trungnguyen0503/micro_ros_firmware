#ifndef ROS_DIFF_DRIVE_NODE_H
#define ROS_DIFF_DRIVE_NODE_H

#include <stdint.h>

void Ros_DiffDriveNode_Init();
void Ros_DiffDriveNode_SpinExec(uint32_t timeout_ns);

extern int l_kp,l_ki,l_kd,l_kff,r_kp,r_ki,r_kd,r_kff;
extern double vel_r,vel_l,vel_r_pid,vel_l_pid;

#endif
