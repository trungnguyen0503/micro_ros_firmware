#include "project/ros/vel_node.h"
#include "project/task/init.h"

#include "cmsis_os2.h"

void StartVelocityTask(void *arg) {
    (void)arg;
    Task_Init_WaitUntilDone();

    while (1) {
        const uint32_t interval_ms = 50;
        const uint32_t interval_ns = interval_ms * 1000000;
        Ros_VelNode_SpinExec(interval_ns);
        Ros_VelNode_PublishVel();
        osDelay(interval_ms);
    }
}
