#ifndef PROJECT_UTILITY_H
#define PROJECT_UTILITY_H

#include "builtin_interfaces/msg/time.h"
#include "rmw_microros/time_sync.h"
#include "rosidl_runtime_c/string.h"

enum Utility_LogLevel {
    Utility_LogInfo,
    Utility_LogWarning,
    Utility_LogError,
    Utility_LogFatal
};

#define Utility_MakeStaticRosCString(literal_string) \
    ((rosidl_runtime_c__String){ \
        .capacity = sizeof(literal_string), \
        .size = sizeof(literal_string) - 1, \
        .data = literal_string, \
    })

// Convert RTOS tick count into ROS' time message
static inline builtin_interfaces__msg__Time Utility_GetRosTimeStamp();

// Calculate the time difference between 2 ROS' time message in nanoseconds (t1 - t2)
static inline int64_t Utility_GetRosTimeDiffNs(
    builtin_interfaces__msg__Time t1,
    builtin_interfaces__msg__Time t2
);

static inline bool Utility_IsRosTimeEqualZero(builtin_interfaces__msg__Time t);

// Raw printing
__printflike(1, 2) void Utility_Printf(const char *fmt, ...);

// Printing with time stamp and log level
__printflike(2, 3) void Utility_Log(enum Utility_LogLevel level, const char *fmt, ...);

#define Utility_LogEx(level, fmt, ...) \
    Utility_Log(level, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

static inline builtin_interfaces__msg__Time Utility_GetRosTimeStamp() {
    const int64_t ns = rmw_uros_epoch_nanos();
    return (builtin_interfaces__msg__Time){
        .sec = (int32_t)(ns / 1000000000),
        .nanosec = (uint32_t)(ns % 1000000000),
    };
}

static inline int64_t Utility_GetRosTimeDiffNs(
    const builtin_interfaces__msg__Time t1,
    const builtin_interfaces__msg__Time t2
) {
    const int64_t t1_ns = ((int64_t)t1.sec * 1000000000) + t1.nanosec;
    const int64_t t2_ns = ((int64_t)t2.sec * 1000000000) + t2.nanosec;
    return t1_ns - t2_ns;
}

static inline bool Utility_IsRosTimeEqualZero(builtin_interfaces__msg__Time t) {
    return t.sec == 0 && t.nanosec == 0;
}

#endif
