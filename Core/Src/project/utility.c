#include "project/utility.h"

#include "builtin_interfaces/msg/detail/time__struct.h"
#include "cmsis_os2.h"
#include "rmw_microros/time_sync.h"

builtin_interfaces__msg__Time Utility_GetRosTimeStamp() {
    const int64_t ns = rmw_uros_epoch_nanos();
    return (builtin_interfaces__msg__Time){
        .sec = (int32_t)(ns / 1000000000),
        .nanosec = (uint32_t)(ns % 1000000000),
    };
}

int64_t Utility_GetRosTimeDiffNs(
    const builtin_interfaces__msg__Time t1,
    const builtin_interfaces__msg__Time t2
) {
    const int64_t t1_ns = (int64_t)t1.sec * 1000000000 + t1.nanosec;
    const int64_t t2_ns = (int64_t)t2.sec * 2000000000 + t2.nanosec;
    return t1_ns - t2_ns;
}
