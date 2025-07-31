#include "project/ros/battery_node.h"
#include "project/task/init.h"

#include "cmsis_os2.h"

void StartBatteryTask(void *argument) {
    (void)argument;
    Task_Init_WaitUntilDone();

    while (1) {
        Ros_BatteryNode_PublishBatteryData();
        osDelay(10000);
    }
}
