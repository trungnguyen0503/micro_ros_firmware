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

#include <math.h>

typedef struct WheelPidCommon {
    double max_output;
    double deadzone;
    double integral_max;
    double hyst;
} WheelPidCommon_t;

typedef struct WheelPid {
    double kp;
    double ki;
    double kd;
    double kff;
    double setpoint;
    double prev_measure;
    double integral;
    double prev_output;
} WheelPid_t;

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

static WheelPidCommon_t g_wheel_pid = { 0 };

static WheelPid_t g_left_wheel_pid = {
    .kp = 500,
    .ki = 4000,
    .kd = 0,
    .kff = 500,
};

static WheelPid_t g_right_wheel_pid = {
    .kp = 500,
    .ki = 4000,
    .kd = 0,
    .kff = 500
};

static double ComputeWheelPid(WheelPid_t *pid, double measure, double dt);
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

    g_wheel_pid.max_output = Kine_AngularVelToLinearVel(Actuator_Motor_MAX_ANGULAR_VEL) * 1.02;
    g_wheel_pid.deadzone =
        Kine_AngularVelToLinearVel(Actuator_Motor_MIN_ANGULAR_VEL) +
        g_wheel_pid.max_output * 0.09;
    g_wheel_pid.integral_max = g_wheel_pid.max_output * 0.5;
    g_wheel_pid.hyst = g_wheel_pid.deadzone * 0.3;
    g_wheel_pid.integral_max = g_wheel_pid.max_output * 0.125;
}

void Ros_DiffDriveNode_SpinExec(const uint32_t timeout_ns) {
    const rcl_ret_t ret = rclc_executor_spin_some(&g_exec, timeout_ns);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "%s executor failed (code %d)", NODE_NAME, (int)ret);
    }
}

static double ComputeWheelPid(WheelPid_t *const pid, const double measure, const double dt) {
    double result = 0;

    const double abs_sp = fabs(pid->setpoint);
    const double err = pid->setpoint - measure;
    const double abs_err = fabs(err);
    if (
        pid->setpoint == 0 ||
        (abs_sp <= g_wheel_pid.deadzone && abs_err <= g_wheel_pid.deadzone)
    ) {
        pid->integral = 0;
        goto Return;
    }

    pid->integral += err * dt;
    if (pid->integral > g_wheel_pid.integral_max) {
        pid->integral = g_wheel_pid.integral_max;
    } else if (pid->integral < -g_wheel_pid.integral_max) {
        pid->integral = -g_wheel_pid.integral_max;
    }

    const double derivative = dt > 0 ? (measure - pid->prev_measure) / dt : 0;
    const double ff = pid->kff / 1000 * pid->setpoint;
    result =
        ff + (pid->kp / 1000 * err) +
        (pid->ki / 1000 * pid->integral) -
        (pid->kd / 1000 * derivative);
    if (fabs(result) < g_wheel_pid.deadzone) {
        if (fabs(pid->prev_output) < g_wheel_pid.deadzone) {
            result = 0;
        } else if (abs_err < g_wheel_pid.deadzone + g_wheel_pid.hyst) {
            result = g_wheel_pid.deadzone * (pid->prev_output > 0 ? 1 : -1);
        }
    } else if (result > g_wheel_pid.max_output) {
        result = g_wheel_pid.max_output;
    } else if (result < -g_wheel_pid.max_output) {
        result = -g_wheel_pid.max_output;
    }
Return:
    pid->prev_measure = measure;
    pid->prev_output = result;
    return result;
}

static void CmdVelCallback(const void *const void_msg) {
    const geometry_msgs__msg__Twist *msg = void_msg;
    const double lv = msg->linear.x;
    const double av = msg->angular.z;
    const double v_left = lv - (av * Kine_WHEEL_BASE / 2);
    const double v_right = lv + (av * Kine_WHEEL_BASE / 2);
    g_left_wheel_pid.setpoint = v_left;
    g_right_wheel_pid.setpoint = v_right;
}

static void VelEncoderCallback(const void *const void_msg) {
    const geometry_msgs__msg__TwistStamped *msg = void_msg;
    const double lv = msg->twist.linear.x;
    const double av = msg->twist.angular.z;
    const int64_t stamp_ns = Utility_RosTimeToNs(msg->header.stamp);
    const double dt = (double)(stamp_ns - g_last_vel_encoder_stamp_ns) / 1000000000;
    const double v_left = lv - (av * Kine_WHEEL_BASE / 2);
    const double v_right = lv + (av * Kine_WHEEL_BASE / 2);
    const double v_left_pid = ComputeWheelPid(&g_left_wheel_pid, v_left, dt);
    const double v_right_pid = ComputeWheelPid(&g_right_wheel_pid, v_right, dt);
    const double w_left = Kine_LinearVelToAngularVel(v_left_pid);
    const double w_right = Kine_LinearVelToAngularVel(v_right_pid);
    Actuator_Motor_SetLeftAngularVel(w_left);
    Actuator_Motor_SetRightAngularVel(w_right);
    g_last_vel_encoder_stamp_ns = stamp_ns;
}
