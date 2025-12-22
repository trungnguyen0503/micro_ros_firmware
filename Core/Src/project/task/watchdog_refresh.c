#include "project/task/init.h"
#include "cmsis_os2.h"
#include "iwdg.h"

extern IWDG_HandleTypeDef hiwdg;

void StartIWDGTask(void *argument) {
    (void)argument;
    while (1) {
        if (ROS_Init_done)
        {
            HAL_IWDG_Refresh(&hiwdg);
        }
        osDelay(10);
    }
}
