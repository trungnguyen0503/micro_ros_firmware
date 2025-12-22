#ifndef TASK_INIT_H
#define TASK_INIT_H

#include <stdbool.h>
extern bool ROS_Init_done;

bool Task_Init_IsDone();
void Task_Init_WaitUntilDone();
#endif
