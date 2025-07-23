#include "project/ros/teleop_node.h"

#include "cmsis_os2.h"

void StartTeleopTask(void *argument) {
    (void)argument;

    while (1) {
        const uint32_t interval_ms = 50;
        const uint32_t interval_ns = interval_ms * 1000;
        rclc_executor_spin_some(Ros_TeleopNode_GetExec(), interval_ns);

        osDelay(interval_ms);
    }
}
