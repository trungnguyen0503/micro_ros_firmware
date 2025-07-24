#include "project/utility.h"

#include "cmsis_os2.h"

builtin_interfaces__msg__Time Utility_GetRosTimeStamp() {
    const uint32_t ms = osKernelGetTickCount();
    return (builtin_interfaces__msg__Time){
        .sec = (int32_t)(ms / 1000),
        .nanosec = (ms % 1000) * 1000000,
    };
}
