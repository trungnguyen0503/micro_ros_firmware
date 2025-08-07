#include "project/ros/diff_drive_node.h"
#include "project/task/init.h"
#include "project/utility.h"

#include "cmsis_os2.h"

void StartDiffDriveTask(void *arg) {
    (void)arg;
    Task_Init_WaitUntilDone();

    while (1) {
        const uint32_t interval_ms = 20;
        // const uint32_t interval_ns = interval_ms * 1000000;

        // const rcl_ret_t ret = rclc_executor_spin_some(Ros_DiffDriveNode_GetExec(), interval_ns);
        // if (ret != RCL_RET_OK) {
        //     Utility_Log(Utility_LogInfo, "Diff drive executor failed (code %d)", (int)ret);
        // }
        osDelay(interval_ms);
    }
}
