#include "project/ros/odometry_node.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/ros/vel_node.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"
#include "project/utility.h"

#include "geometry_msgs/msg/twist_stamped.h"
#include "nav_msgs/msg/odometry.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"
#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#include <math.h>

#define FRAME_ID_CAPACITY 16
#define NODE_NAME "odometry_node"

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_vel_data_sub = { 0 };
static rcl_subscription_t g_imu_data_sub = { 0 };
static rcl_subscription_t g_mag_data_sub = { 0 };
static rcl_publisher_t g_odo_data_pub = { 0 };
static rclc_executor_t g_exec = { 0 };

static geometry_msgs__msg__TwistStamped g_vel_data_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};

static sensor_msgs__msg__Imu g_imu_data_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};

static sensor_msgs__msg__MagneticField g_mag_data_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};

static double g_initial_heading = 0;
static int64_t g_odo_prev_stamp_ns = 0;
static double g_odo_x = 0;
static double g_odo_y = 0;
static double g_odo_yaw = 0;

static void VelDataCallback(const void *void_msg);
static void ImuDataCallback(const void *void_msg);
static void MagDataCallback(const void *void_msg);
static void PublishOdoData();

void Ros_OdometryNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &g_vel_data_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TwistStamped),
        Ros_VelNode_VEL_DATA_TOPIC
    );
    rclc_subscription_init_best_effort(
        &g_imu_data_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        Ros_ImuNode_IMU_DATA_TOPIC
    );
    rclc_subscription_init_best_effort(
        &g_mag_data_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField),
        Ros_ImuNode_MAG_DATA_TOPIC
    );
    rclc_publisher_init_best_effort(
        &g_odo_data_pub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        Ros_OdometryNode_ODOMETRY_DATA_TOPIC
    );
    rclc_executor_init(&g_exec, &Ros_GetSupportStruct()->context, 3, Ros_GetAllocator());
    rclc_executor_set_trigger(&g_exec, rclc_executor_trigger_all, NULL);
    rclc_executor_add_subscription(
        &g_exec, &g_vel_data_sub,
        &g_vel_data_msg, VelDataCallback, ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec, &g_imu_data_sub,
        &g_imu_data_msg, ImuDataCallback, ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec, &g_mag_data_sub,
        &g_mag_data_msg, MagDataCallback, ON_NEW_DATA
    );
}

void Ros_OdometryNode_RecordInitialHeading() {
    const geometry_msgs__msg__Vector3 mag = Sensor_Imu_GetMag();
    g_initial_heading = atan2(mag.y, mag.x);
}

void Ros_OdometryNode_SpinExec(const uint32_t timeout_ns) {
    const rcl_ret_t ret = rclc_executor_spin_some(&g_exec, timeout_ns);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "%s executor failed (code %d)", NODE_NAME, (int)ret);
    }
}

static void VelDataCallback(const void *void_msg) {
    (void)void_msg;
}

static void ImuDataCallback(const void *void_msg) {
    (void)void_msg;
}

static void MagDataCallback(const void *void_msg) {
    (void)void_msg;
    PublishOdoData();
}

static void PublishOdoData() {
    const __auto_type vel = &g_vel_data_msg;
    const __auto_type imu = &g_imu_data_msg;
    const __auto_type mag = &g_mag_data_msg;
    const int64_t stamp_avg_ns = ({
        const double s =
            ((double)Utility_RosTimeToNs(vel->header.stamp) / 3.0) +
            ((double)Utility_RosTimeToNs(imu->header.stamp) / 3.0) +
            ((double)Utility_RosTimeToNs(mag->header.stamp) / 3.0);
        (int64_t)s;
    });
    const double dt = (double)(stamp_avg_ns - g_odo_prev_stamp_ns) / 1000000000;
    {
        const double lv = vel->twist.linear.x;
        g_odo_x += lv * cos(g_odo_yaw) * dt;
        g_odo_y += lv * sin(g_odo_yaw) * dt;
    }
    {
        const double gyro_weight = 0.5;
        const double mag_y = mag->magnetic_field.y;
        const double mag_x = mag->magnetic_field.x;
        const double mag_yaw = atan2(mag_y, mag_x) - g_initial_heading;
        const double gyro_delta_yaw = imu->angular_velocity.z * dt;
        g_odo_yaw = (g_odo_yaw + gyro_delta_yaw) * gyro_weight + mag_yaw * (1 - gyro_weight);
    }

    const nav_msgs__msg__Odometry msg = {
        .header.stamp = Utility_NsToRosTime(stamp_avg_ns),
        .header.frame_id = Utility_MakeStaticRosCString("odom"),
        .child_frame_id = vel->header.frame_id,
        .twist.twist = vel->twist,
        .pose.pose.position.x = g_odo_x,
        .pose.pose.position.y = g_odo_y,
        .pose.pose.orientation.z = sin(g_odo_yaw / 2),
        .pose.pose.orientation.w = cos(g_odo_yaw / 2),
        .twist.covariance[0] = -1,
        .pose.covariance[0] = -1,
    };

    const rcl_ret_t ret = rcl_publish(&g_odo_data_pub, &msg, NULL);
    if (ret != RCL_RET_OK) {
        Utility_Log(
            Utility_LogWarning, "%s failed to publish %s (code %d)",
            NODE_NAME, Ros_OdometryNode_ODOMETRY_DATA_TOPIC, (int)ret
        );
    }
}
