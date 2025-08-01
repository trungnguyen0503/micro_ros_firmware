#include "project/ros/odometry_node.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"
#include "project/utility.h"

#include "nav_msgs/msg/odometry.h"
#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#include <math.h>

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_imu_data_sub = { 0 };
static rcl_subscription_t g_mag_data_sub = { 0 };
static rcl_publisher_t g_odo_data_pub = { 0 };
static rclc_executor_t g_exec = { 0 };
static sensor_msgs__msg__Imu g_imu_data_msg = { 0 };
static sensor_msgs__msg__MagneticField g_mag_data_msg = { 0 };

static double g_initial_heading = 0;
static builtin_interfaces__msg__Time g_odo_prev_stamp = { 0 };
static geometry_msgs__msg__Vector3 g_odo_pos = { 0 };
static double g_odo_yaw = 0;

void Ros_OdometryNode_Init() {
    rclc_node_init_default(&g_node, "odometry_node", "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &g_imu_data_sub,
        &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        "imu/data"
    );
    rclc_subscription_init_best_effort(
        &g_mag_data_sub,
        &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField),
        "magnetic_field/data"
    );
    rclc_publisher_init_best_effort(
        &g_odo_data_pub,
        &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        "odometry/data"
    );
    rclc_executor_init(
        &g_exec, &Ros_GetSupportStruct()->context,
        2, Ros_GetAllocator()
    );
    rclc_executor_add_subscription(
        &g_exec,
        &g_imu_data_sub,
        &g_imu_data_msg,
        Ros_OdometryNode_ImuDataCallback,
        ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec,
        &g_mag_data_sub,
        &g_mag_data_msg,
        Ros_OdometryNode_MagDataCallback,
        ON_NEW_DATA
    );
}

void Ros_OdometryNode_RecordInitialHeading() {
    const geometry_msgs__msg__Vector3 mag = Sensor_Imu_GetMag();
    g_initial_heading = atan2(mag.y, mag.x);
}

const rcl_node_t *Ros_OdometryNode_GetHandle() {
    return &g_node;
}

const rcl_subscription_t *Ros_OdometryNode_GetImuDataSub() {
    return &g_imu_data_sub;
}

const rcl_subscription_t *Ros_OdometryNode_GetMagDataSub() {
    return &g_mag_data_sub;
}

const rcl_publisher_t *Ros_OdometryNode_GetOdoDataPub() {
    return &g_odo_data_pub;
}

rclc_executor_t *Ros_OdometryNode_GetExec() {
    return &g_exec;
}

void Ros_OdometryNode_ImuDataCallback(const void *void_msg) {
    (void)void_msg;
}

void Ros_OdometryNode_MagDataCallback(const void *void_msg) {
    (void)void_msg;
}

void Ros_OdometryNode_PublishOdoData() {
    const sensor_msgs__msg__Imu *imu = &g_imu_data_msg;
    const sensor_msgs__msg__MagneticField *mag = &g_mag_data_msg;

    const double dt = (double)Utility_GetRosTimeDiffNs(mag->header.stamp, g_odo_prev_stamp) / 1e9;

    const double v_scalar = ({
        const double wl = Sensor_MotorEncoder_GetLeftAngularVel();
        const double wr = Sensor_MotorEncoder_GetRightAngularVel();
        const double vl = Kinematics_AngularVelToLinearVel(wl);
        const double vr = Kinematics_AngularVelToLinearVel(wr);
        (vl + vr) / 2.0;
    });
    const double delta_gyro_yaw = imu->angular_velocity.z * dt;
    const double mag_yaw = atan2(mag->magnetic_field.y, mag->magnetic_field.x);
    const double gyro_weight = 0.5;
    const double yaw = gyro_weight * (g_odo_yaw + delta_gyro_yaw) + (1 - gyro_weight) * mag_yaw;
    const double delta_yaw = yaw - g_odo_yaw;
    Utility_Log(Utility_LogInfo, "delta_yaw: %lf", delta_yaw);
    g_odo_yaw = yaw;
    g_odo_pos.x += v_scalar * cos(g_odo_yaw) * dt;
    g_odo_pos.y += v_scalar * sin(g_odo_yaw) * dt;
    g_odo_prev_stamp = mag->header.stamp;

    nav_msgs__msg__Odometry msg = {
        .header.stamp = Utility_GetRosTimeStamp(),
        .header.frame_id = Utility_MakeStaticRosCString("odom"),
        .child_frame_id = Utility_MakeStaticRosCString("base_link"),
        .twist.twist.linear.x = v_scalar,
        .twist.twist.angular.z = delta_yaw,
        .pose.pose.position.x = g_odo_pos.x,
        .pose.pose.position.y = g_odo_pos.y,
        .pose.pose.orientation.z = sin(g_odo_yaw / 2),
        .pose.pose.orientation.w = cos(g_odo_yaw / 2),
    };

    const rcl_ret_t ret = rcl_publish(Ros_OdometryNode_GetOdoDataPub(), &msg, NULL);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "Odometry publisher failed (code %d)", ret);
    }
}
