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
        Ros_OdometryNode_SpinExec(interval_ns);
        Ros_OdometryNode_PublishOdoData();
        osDelay(interval_ms);
    }
}
