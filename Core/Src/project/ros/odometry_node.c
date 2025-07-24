#include "project/ros/odometry_node.h"
#include "project/ros/global.h"

#include "nav_msgs/msg/odometry.h"
#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#define MSG_BUF_SIZE 8

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_imu_data_sub = { 0 };
static rcl_subscription_t g_mag_data_sub = { 0 };
static rcl_publisher_t g_odo_data_pub = { 0 };
static rclc_executor_t g_exec = { 0 };
static sensor_msgs__msg__Imu g_imu_data_msg = { 0 };
static sensor_msgs__msg__MagneticField g_mag_data_msg = { 0 };

static sensor_msgs__msg__Imu g_imu_buf[MSG_BUF_SIZE] = { 0 };
static uint8_t g_imu_buf_idx = 0;

static sensor_msgs__msg__MagneticField g_mag_buf[MSG_BUF_SIZE] = { 0 };
static uint8_t g_mag_buf_idx = 0;

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
    const sensor_msgs__msg__Imu *msg = void_msg;
    g_imu_buf_idx = (g_imu_buf_idx + 1) % MSG_BUF_SIZE;
    g_imu_buf[g_imu_buf_idx] = *msg;
}

void Ros_OdometryNode_MagDataCallback(const void *void_msg) {
    const sensor_msgs__msg__MagneticField *msg = void_msg;
    g_mag_buf_idx = (g_mag_buf_idx + 1) % MSG_BUF_SIZE;
    g_mag_buf[g_mag_buf_idx] = *msg;
}
