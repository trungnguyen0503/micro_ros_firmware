#include "project/ros/teleop_node.h"
#include "project/actuator/motor.h"

#include "rclc/executor.h"
#include "rclc/rclc.h"

#include "cmsis_os.h"

void StartSubscribeTask(void *argument) {
    UNUSED(argument);

    while (1) {
        const uint32_t interval_ms = 50;
        const uint32_t interval_ns = interval_ms * 1000;

        rclc_executor_spin_some(&Ros_Teleop_exec, interval_ns);

        osDelay(interval_ms);
    }
}
