#ifndef ROS_GLOBAL_H
#define ROS_GLOBAL_H

#include "rcl/allocator.h"
#include "rclc/types.h"

// Initialize Uros' transporter, allocator and support struct
void Ros_Init();
const rcl_allocator_t *Ros_GetAllocator();
rclc_support_t *Ros_GetSupportStruct();

#endif
