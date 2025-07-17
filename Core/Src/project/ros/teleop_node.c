#include "project/ros/teleop_node.h"
#include "project/actuator/motor.h"

rcl_node_t Ros_Teleop_node = { 0 };
rcl_subscription_t Ros_Teleop_data_sub = { 0 };
rclc_executor_t Ros_Teleop_exec = { 0 };
geometry_msgs__msg__Twist Ros_Teleop_msg = { 0 };

static uint32_t DEBUG_teleop_cb_cnt = 0;

void Ros_Teleop_Callback(const void *void_msg) {
    const geometry_msgs__msg__Twist *msg = void_msg;
    DEBUG_teleop_cb_cnt++;
    UNUSED(msg);
}
