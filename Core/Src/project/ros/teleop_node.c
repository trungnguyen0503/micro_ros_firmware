#include "project/ros/teleop_node.h"
#include "project/actuator/motor.h"
#include "project/kinematics.h"
#include "project/ros/global.h"

rcl_node_t Ros_Teleop_node = { 0 };
rcl_subscription_t Ros_Teleop_data_sub = { 0 };
rclc_executor_t Ros_Teleop_exec = { 0 };
geometry_msgs__msg__Twist Ros_Teleop_msg = { 0 };

static uint32_t DEBUG_teleop_cb_cnt = 0;

void Ros_Teleop_InitNode() {
    rclc_node_init_default(&Ros_Teleop_node, "teleop_node", "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &Ros_Teleop_data_sub,
        &Ros_Teleop_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel"
    );
    rclc_executor_init(&Ros_Teleop_exec, &Ros_GetSupportStruct()->context, 1, Ros_GetAllocator());
    rclc_executor_add_subscription(
        &Ros_Teleop_exec, &Ros_Teleop_data_sub, &Ros_Teleop_msg,
        Ros_Teleop_Callback, ON_NEW_DATA
    );
}

void Ros_Teleop_Callback(const void *void_msg) {
    const geometry_msgs__msg__Twist *msg = void_msg;
    const double v = msg->linear.x;
    const double w = msg->angular.z;
    const double v_left = v - w * Kinematics_WHEEL_BASE / 2.0;
    const double v_right = v + w * Kinematics_WHEEL_BASE / 2.0;
    const double w_left = Kinematics_LinearVelToAngularVel(v_left);
    const double w_right = Kinematics_LinearVelToAngularVel(v_right);
    Actuator_Motor_SetLeftAngularVel(w_left);
    Actuator_Motor_SetRightAngularVel(w_right);
    DEBUG_teleop_cb_cnt++;
    UNUSED(msg);
}
