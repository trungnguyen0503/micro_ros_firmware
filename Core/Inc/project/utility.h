#ifndef PROJECT_UTILITY_H
#define PROJECT_UTILITY_H

#include "builtin_interfaces/msg/time.h"

// Convert RTOS tick count into ROS' time message
builtin_interfaces__msg__Time Utility_GetRosTimeStamp();

#endif
