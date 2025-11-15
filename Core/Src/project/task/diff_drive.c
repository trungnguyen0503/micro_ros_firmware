#include "project/ros/diff_drive_node.h"
#include "project/task/init.h"
#include "project/utility.h"

#include "rcl/time.h"

#include "cmsis_os2.h"

void StartDiffDriveTask(void *arg) {
    (void)arg;
    Task_Init_WaitUntilDone();

    while (1) {
        Ros_DiffDriveNode_SpinExec(RCL_MS_TO_NS(25));
    }
}
