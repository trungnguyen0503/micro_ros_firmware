#include "project/ros/odometry_node.h"
#include "project/task/init.h"
#include "project/utility.h"

#include "cmsis_os2.h"

void StartOdometryTask(void *argument) {
    (void)argument;
    Task_Init_WaitUntilDone();

    while (1) {
        Ros_OdometryNode_SpinExec(RCL_MS_TO_NS(50));
    }
}
