#include "project/ros/global.h"
#include "project/uros/utility.h"
#include "project/utility.h"

#include "rclc/rclc.h"
#include "rmw_microros/rmw_microros.h"

#include "usart.h"

static rcl_allocator_t Ros_allocator = { 0 };
static rclc_support_t Ros_support = { 0 };

void Ros_Init() {
    // Set UART DMA transport
    {
        const rmw_ret_t ret = rmw_uros_set_custom_transport(
            true,
            &huart4,
            Uros_UartDmaTransport_Open,
            Uros_UartDmaTransport_Close,
            Uros_UartDmaTransport_Write,
            Uros_UartDmaTransport_Read
        );
        if (ret != RMW_RET_OK) {
            Utility_Log(Utility_LogFatal, "Failed to initialize ros transport");
            while (1) {
            }
        }
    }
    // Set Uros custom allocator
    {
        Ros_allocator.allocate = Uros_Allocate;
        Ros_allocator.deallocate = Uros_Deallocate;
        Ros_allocator.reallocate = Uros_Reallocate;
        Ros_allocator.zero_allocate = Uros_ZeroAllocate;
        if (!rcutils_set_default_allocator(&Ros_allocator)) {
            Utility_Log(Utility_LogFatal, "Failed to initialize allocator for ros");
            while (1) {
            }
        }
    }
    // Set support struct
    {
        const rcl_ret_t ret = rclc_support_init(&Ros_support, 0, NULL, &Ros_allocator);
        if (ret != RCL_RET_OK) {
            Utility_Log(Utility_LogFatal, "Failed to initialize support struct for ros");
            while (1) {
            }
        }
    }
    // Sync time with agent using NTP
    {
        const rmw_ret_t ret = rmw_uros_sync_session(1000);
        if (ret != RMW_RET_OK) {
            Utility_Log(Utility_LogWarning, "Failed to sync time with agent");
        }
    }
}

const rcl_allocator_t *Ros_GetAllocator() {
    return &Ros_allocator;
}

rclc_support_t *Ros_GetSupportStruct() {
    return &Ros_support;
}
