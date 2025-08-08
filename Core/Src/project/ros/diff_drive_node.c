#include "project/ros/diff_drive_node.h"
#include "project/actuator/motor.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/utility.h"

#include "rclc/executor.h"
#include "rclc/node.h"
#include "rclc/subscription.h"
#include "std_msgs/msg/float64.h"

#define NODE_NAME "diff_drive_node"
#define CMD_VEL_LEFT_TOPIC "cmd_vel_left"
#define CMD_VEL_RIGHT_TOPIC "cmd_vel_right"

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_cmd_vel_left_sub = { 0 };
static rcl_subscription_t g_cmd_vel_right_sub = { 0 };
static rclc_executor_t g_exec = { 0 };

static std_msgs__msg__Float64 g_cmd_vel_left_msg = { 0 };
static std_msgs__msg__Float64 g_cmd_vel_right_msg = { 0 };

static void CmdVelLeftCallback(const void *void_msg);
static void CmdVelRightCallback(const void *void_msg);

void Ros_DiffDriveNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "", Ros_GetSupportStruct());
    const rmw_qos_profile_t sub_qos_profile = {
        .avoid_ros_namespace_conventions = false,
        .deadline = RMW_DURATION_UNSPECIFIED,
        .depth = 10,
        .durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL,
        .history = RMW_QOS_POLICY_HISTORY_KEEP_LAST,
        .lifespan = RMW_DURATION_UNSPECIFIED,
        .liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC,
        .liveliness_lease_duration = RMW_DURATION_UNSPECIFIED,
        .reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE,
    };
    rclc_subscription_init(
        &g_cmd_vel_left_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        CMD_VEL_LEFT_TOPIC,
        &sub_qos_profile
    );
    rclc_subscription_init(
        &g_cmd_vel_right_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        CMD_VEL_RIGHT_TOPIC,
        &sub_qos_profile
    );
    rclc_executor_init(&g_exec, &Ros_GetSupportStruct()->context, 2, Ros_GetAllocator());
    rclc_executor_add_subscription(
        &g_exec, &g_cmd_vel_left_sub,
        &g_cmd_vel_left_msg, CmdVelLeftCallback, ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec, &g_cmd_vel_right_sub,
        &g_cmd_vel_right_msg, CmdVelRightCallback, ON_NEW_DATA
    );
}

void Ros_DiffDriveNode_SpinExec(const uint32_t timeout_ns) {
    const rcl_ret_t ret = rclc_executor_spin_some(&g_exec, timeout_ns);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "%s executor failed (code %d)", NODE_NAME, (int)ret);
    }
}

static void CmdVelLeftCallback(const void *const void_msg) {
    Utility_Log(Utility_LogInfo, "%s received", CMD_VEL_LEFT_TOPIC);
    const std_msgs__msg__Float64 *msg = void_msg;
    const double v = msg->data;
    const double w = Kinematics_LinearVelToAngularVel(v);
    Actuator_Motor_SetLeftAngularVel(w);
}

static void CmdVelRightCallback(const void *const void_msg) {
    Utility_Log(Utility_LogInfo, "%s received", CMD_VEL_RIGHT_TOPIC);
    const std_msgs__msg__Float64 *msg = void_msg;
    const double v = msg->data;
    const double w = Kinematics_LinearVelToAngularVel(v);
    Actuator_Motor_SetRightAngularVel(w);
}
