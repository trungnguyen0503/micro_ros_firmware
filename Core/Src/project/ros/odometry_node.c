#include "project/ros/odometry_node.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/ros/vel_node.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"
#include "project/utility.h"

#include "geometry_msgs/msg/twist_stamped.h"
#include "nav_msgs/msg/odometry.h"

#include <math.h>

#define FRAME_ID_CAPACITY 16
#define NODE_NAME "odometry_node"

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_vel_data_sub = { 0 };
static rcl_publisher_t g_odo_data_pub = { 0 };
static rclc_executor_t g_exec = { 0 };

static geometry_msgs__msg__TwistStamped g_vel_data_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};

static builtin_interfaces__msg__Time g_odo_prev_stamp = { 0 };
static double g_odo_x = 0;
static double g_odo_y = 0;
static double g_odo_yaw = 0;

static void VelDataCallback(const void *void_msg);

void Ros_OdometryNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &g_vel_data_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TwistStamped),
        Ros_VelNode_VEL_DATA_TOPIC
    );
    rclc_publisher_init_best_effort(
        &g_odo_data_pub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        Ros_OdometryNode_ODOMETRY_DATA_TOPIC
    );
    rclc_executor_init(&g_exec, &Ros_GetSupportStruct()->context, 1, Ros_GetAllocator());
    rclc_executor_add_subscription(
        &g_exec, &g_vel_data_sub,
        &g_vel_data_msg, VelDataCallback, ON_NEW_DATA
    );
}

void Ros_OdometryNode_SpinExec(const uint32_t timeout_ns) {
    rclc_executor_spin_some(&g_exec, timeout_ns);
}

void Ros_OdometryNode_PublishOdoData() {
    const geometry_msgs__msg__TwistStamped *vel = &g_vel_data_msg;
    if (Utility_IsRosTimeEqualZero(vel->header.stamp)) {
        return;
    }
    const double dt =
        (double)Utility_GetRosTimeDiffNs(vel->header.stamp, g_odo_prev_stamp) /
        1000000000.0;
    const double lv = vel->twist.linear.x;
    const double av = vel->twist.angular.z;
    g_odo_yaw += av;
    g_odo_x += lv * cos(g_odo_yaw) * dt;
    g_odo_y += lv * sin(g_odo_yaw) * dt;

    const nav_msgs__msg__Odometry msg = {
        .header.stamp = vel->header.stamp,
        .header.frame_id = Utility_MakeStaticRosCString("odom"),
        .child_frame_id = vel->header.frame_id,
        .twist.twist.linear.x = lv,
        .twist.twist.angular.z = av,
        .pose.pose.position.x = g_odo_x,
        .pose.pose.position.y = g_odo_y,
        .pose.pose.orientation.z = sin(g_odo_yaw / 2),
        .pose.pose.orientation.w = cos(g_odo_yaw / 2),
        .twist.covariance[0] = -1,
        .pose.covariance[0] = -1,
    };

    const rcl_ret_t ret = rcl_publish(&g_odo_data_pub, &msg, NULL);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "Odometry publisher failed (code %d)", ret);
    }
}

static void VelDataCallback(const void *void_msg) {
    (void)void_msg;
}
