#ifndef PROJECT_UTILITY_H
#define PROJECT_UTILITY_H

#include "builtin_interfaces/msg/time.h"
#include "rosidl_runtime_c/string.h"

#define Utility_MakeStaticRosCString(literal_string) \
    ((rosidl_runtime_c__String){ \
        .capacity = sizeof(literal_string), \
        .size = sizeof(literal_string) - 1, \
        .data = literal_string, \
    })

// Convert RTOS tick count into ROS' time message
builtin_interfaces__msg__Time Utility_GetRosTimeStamp();

// Calculate the time difference between 2 ROS' time message in nanoseconds (t1 - t2)
int64_t Utility_GetRosTimeDiffNs(
    builtin_interfaces__msg__Time t1,
    builtin_interfaces__msg__Time t2
);

#endif
