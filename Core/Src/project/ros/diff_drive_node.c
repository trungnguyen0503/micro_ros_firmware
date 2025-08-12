#include "project/ros/diff_drive_node.h"
#include "project/actuator/motor.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/ros/vel_node.h"
#include "project/utility.h"

#include "geometry_msgs/msg/twist.h"
#include "geometry_msgs/msg/twist_stamped.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"

typedef struct PID {
    double kp;
    double ki;
    double kd;
    double setpoint;
    double prev_error;
    double integral;
} PID;

#define NODE_NAME "diff_drive_node"
#define CMD_VEL_TOPIC "cmd_vel"

enum {
    FRAME_ID_CAPACITY = 16
};

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_cmd_vel_sub = { 0 };
static rcl_subscription_t g_vel_encoder_sub = { 0 };
static rclc_executor_t g_exec = { 0 };

static geometry_msgs__msg__Twist g_cmd_vel_pid_msg = { 0 };
static geometry_msgs__msg__TwistStamped g_vel_encoder_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};

static int64_t g_last_vel_encoder_stamp_ns = 0;

static PID g_linear_vel_pid = {
    .kp = 0,
    .ki = 0,
    .kd = 0,
};

static PID g_angular_vel_pid = {
    .kp = 0,
    .ki = 0,
    .kd = 0,
};

static double PID_Compute(PID *pid, double observation, double dt);
static void CmdVelCallback(const void *void_msg);
static void VelEncoderCallback(const void *void_msg);

void Ros_DiffDriveNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &g_cmd_vel_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        CMD_VEL_TOPIC
    );
    rclc_subscription_init_best_effort(
        &g_vel_encoder_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TwistStamped),
        Ros_VelNode_VEL_DATA_TOPIC
    );
    rclc_executor_init(&g_exec, &Ros_GetSupportStruct()->context, 2, Ros_GetAllocator());
    rclc_executor_add_subscription(
        &g_exec, &g_cmd_vel_sub,
        &g_cmd_vel_pid_msg, CmdVelCallback, ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec, &g_vel_encoder_sub,
        &g_vel_encoder_msg, VelEncoderCallback, ON_NEW_DATA
    );
}

void Ros_DiffDriveNode_SpinExec(const uint32_t timeout_ns) {
    const rcl_ret_t ret = rclc_executor_spin_some(&g_exec, timeout_ns);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "%s executor failed (code %d)", NODE_NAME, (int)ret);
    }
}

static double PID_Compute(PID *const pid, const double observation, const double dt) {
    const double error = pid->setpoint - observation;
    pid->integral += error * dt;
    const double derivative = dt > 0 ? (error - pid->prev_error) / dt : 0;
    pid->prev_error = error;
    return (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative);
}

static void CmdVelCallback(const void *const void_msg) {
    Utility_Log(Utility_LogInfo, "%s received", CMD_VEL_TOPIC);
    const geometry_msgs__msg__Twist *msg = void_msg;
    g_linear_vel_pid.setpoint = msg->linear.x;
    g_angular_vel_pid.setpoint = msg->angular.z;
}

static void VelEncoderCallback(const void *const void_msg) {
    const geometry_msgs__msg__TwistStamped *msg = void_msg;
    const double lv = msg->twist.linear.x;
    const double av = msg->twist.angular.z;
    const int64_t stamp_ns = Utility_RosTimeToNs(msg->header.stamp);
    const double dt = (double)(stamp_ns - g_last_vel_encoder_stamp_ns) / 1000000000;
    const double lv_pid = PID_Compute(&g_linear_vel_pid, lv, dt);
    const double av_pid = PID_Compute(&g_angular_vel_pid, av, dt);
    const double v_left = lv_pid - (av_pid * Kinematics_WHEEL_BASE / 2);
    const double v_right = lv_pid + (av_pid * Kinematics_WHEEL_BASE / 2);
    const double w_left = Kinematics_LinearVelToAngularVel(v_left);
    const double w_right = Kinematics_LinearVelToAngularVel(v_right);
    Actuator_Motor_SetLeftAngularVel(w_left);
    Actuator_Motor_SetRightAngularVel(w_right);
    g_last_vel_encoder_stamp_ns = stamp_ns;
}
