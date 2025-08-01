#include "project/ros/odometry_node.h"
#include "project/task/init.h"
#include "project/utility.h"

#include "cmsis_os2.h"

void StartOdometryTask(void *argument) {
    (void)argument;
    Task_Init_WaitUntilDone();

    while (1) {
        const uint32_t interval_ms = 50;
        const uint32_t interval_ns = interval_ms * 1000000;

        const rcl_ret_t ret = rclc_executor_spin_some(Ros_OdometryNode_GetExec(), interval_ns);
        if (ret != RCL_RET_OK) {
            Utility_Log(Utility_LogWarning, "Odometry executor failed (code %d)", ret);
        }
        Ros_OdometryNode_PublishOdoData();

        osDelay(interval_ms);
    }
}
