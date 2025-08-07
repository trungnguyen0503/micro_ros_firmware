#include "project/ros/diff_drive_node.h"
#include "project/actuator/motor.h"
#include "project/kinematics.h"
#include "project/ros/global.h"

#include "project/utility.h"
#include "std_msgs/msg/float64.h"

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_cmd_vel_left_sub = { 0 };
static rcl_subscription_t g_cmd_vel_right_sub = { 0 };
static rclc_executor_t g_exec = { 0 };

static std_msgs__msg__Float64 g_cmd_vel_left_msg = { 0 };
static std_msgs__msg__Float64 g_cmd_vel_right_msg = { 0 };

void Ros_DiffDriveNode_Init() {
    rclc_node_init_default(&g_node, "diff_drive_node", "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &g_cmd_vel_left_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "cmd_vel_left"
    );
    rclc_subscription_init_best_effort(
        &g_cmd_vel_right_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "cmd_vel_right"
    );
    rclc_executor_init(&g_exec, &Ros_GetSupportStruct()->context, 2, Ros_GetAllocator());
    rclc_executor_add_subscription(
        &g_exec, &g_cmd_vel_left_sub, &g_cmd_vel_left_msg,
        Ros_DiffDriveNode_CmdVelLeftCallback, ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec, &g_cmd_vel_right_sub, &g_cmd_vel_right_msg,
        Ros_DiffDriveNode_CmdVelRightCallback, ON_NEW_DATA
    );
}

void Ros_DiffDriveNode_CmdVelLeftCallback(const void *void_msg) {
    Utility_Log(Utility_LogInfo, "cmd_left received");
    const std_msgs__msg__Float64 *msg = void_msg;
    const double v = msg->data;
    const double w = Kinematics_LinearVelToAngularVel(v);
    Actuator_Motor_SetLeftAngularVel(w);
}

void Ros_DiffDriveNode_CmdVelRightCallback(const void *void_msg) {
    Utility_Log(Utility_LogInfo, "cmd_right received");
    const std_msgs__msg__Float64 *msg = void_msg;
    const double v = msg->data;
    const double w = Kinematics_LinearVelToAngularVel(v);
    Actuator_Motor_SetRightAngularVel(w);
}

const rcl_node_t *Ros_DiffDriveNode_GetHandle() {
    return &g_node;
}

const rcl_subscription_t *Ros_DiffDriveNode_GetCmdVelLeftSub() {
    return &g_cmd_vel_left_sub;
}

const rcl_subscription_t *Ros_DiffDriveNode_GetCmdVelRightSub() {
    return &g_cmd_vel_right_sub;
}

rclc_executor_t *Ros_DiffDriveNode_GetExec() {
    return &g_exec;
}
