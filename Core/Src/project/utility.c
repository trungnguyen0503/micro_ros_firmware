#include "project/utility.h"

#include "cmsis_os2.h"
#include "rmw_microros/time_sync.h"

builtin_interfaces__msg__Time Utility_GetRosTimeStamp() {
    const int64_t ns = rmw_uros_epoch_nanos();
    return (builtin_interfaces__msg__Time){
        .sec = (int32_t)(ns / 1000000000),
        .nanosec = (uint32_t)(ns % 1000000000),
    };
}
