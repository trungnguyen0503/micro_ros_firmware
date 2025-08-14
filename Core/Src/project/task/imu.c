#include "project/ros/imu_node.h"
#include "project/task/init.h"

#include "cmsis_os2.h"

void StartIMUTask(void *argument) {
    (void)argument;
    Task_Init_WaitUntilDone();

    while (1) {
        Ros_ImuNode_PublishImuData();
        Ros_ImuNode_PublishMagData();
        osDelay(35);
    }
}
