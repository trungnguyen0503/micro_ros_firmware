#include "project/ros/vel_node.h"
#include "project/task/init.h"

#include "cmsis_os2.h"

void StartVelocityTask(void *arg) {
    (void)arg;
    Task_Init_WaitUntilDone();

    while (1) {
        Ros_VelNode_PublishVel();
        osDelay(40);
    }
}
