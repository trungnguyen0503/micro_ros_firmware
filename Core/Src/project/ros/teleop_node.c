#include "project/ros/teleop_node.h"
#include "project/actuator/motor.h"
#include "project/kinematics.h"
#include "project/ros/global.h"

static rcl_node_t Ros_TeleopNode_handle = { 0 };
static rcl_subscription_t Ros_TeleopNode_teleop_data_sub = { 0 };
static rclc_executor_t Ros_TeleopNode_exec = { 0 };
static geometry_msgs__msg__Twist Ros_Teleop_teleop_data_msg = { 0 };

void Ros_TeleopNode_Init() {
    rclc_node_init_default(&Ros_TeleopNode_handle, "teleop_node", "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &Ros_TeleopNode_teleop_data_sub,
        &Ros_TeleopNode_handle,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel"
    );
    rclc_executor_init(
        &Ros_TeleopNode_exec, &Ros_GetSupportStruct()->context,
        1, Ros_GetAllocator()
    );
    rclc_executor_add_subscription(
        &Ros_TeleopNode_exec, &Ros_TeleopNode_teleop_data_sub, &Ros_Teleop_teleop_data_msg,
        Ros_TeleopNode_Callback, ON_NEW_DATA
    );
}

const rcl_node_t *Ros_TeleopNode_GetHandle() {
    return &Ros_TeleopNode_handle;
}

const rcl_subscription_t *Ros_TeleopNode_GetTeleopDataSub() {
    return &Ros_TeleopNode_teleop_data_sub;
}

rclc_executor_t *Ros_TeleopNode_GetExec() {
    return &Ros_TeleopNode_exec;
}

void Ros_TeleopNode_Callback(const void *void_msg) {
    const geometry_msgs__msg__Twist *msg = void_msg;
    const double v = msg->linear.x;
    const double w = msg->angular.z;
    const double v_left = v - w * Kinematics_WHEEL_BASE / 2.0;
    const double v_right = v + w * Kinematics_WHEEL_BASE / 2.0;
    const double w_left = Kinematics_LinearVelToAngularVel(v_left);
    const double w_right = Kinematics_LinearVelToAngularVel(v_right);
    Actuator_Motor_SetLeftAngularVel(w_left);
    Actuator_Motor_SetRightAngularVel(w_right);
}
