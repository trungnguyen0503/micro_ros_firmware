#include "project/ros/vel_node.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"
#include "project/utility.h"

#include "geometry_msgs/msg/twist_stamped.h"
#include "rclc/executor.h"
#include "rclc/node.h"
#include "rclc/publisher.h"
#include "rclc/subscription.h"
#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#include <math.h>

#define FRAME_ID_CAPACITY 16
#define NODE_NAME "vel_node"
#define IMU_DATA_TOPIC "imu/data"
#define MAG_DATA_TOPIC "magnetic_field/data"

static rcl_node_t g_node = { 0 };
static rcl_subscription_t g_imu_data_sub = { 0 };
static rcl_subscription_t g_mag_data_sub = { 0 };
static rclc_executor_t g_exec = { 0 };
static rcl_publisher_t g_vel_data_pub = { 0 };

static sensor_msgs__msg__Imu g_imu_data_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};
static sensor_msgs__msg__MagneticField g_mag_data_msg = {
    .header.frame_id.data = (char[FRAME_ID_CAPACITY]){ 0 },
    .header.frame_id.capacity = FRAME_ID_CAPACITY,
};

static double g_last_heading = 0;
static builtin_interfaces__msg__Time g_last_vel_stamp = { 0 };

static void ImuDataCallback(const void *void_msg);
static void MagDataCallback(const void *void_msg);

void Ros_VelNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "", Ros_GetSupportStruct());
    rclc_subscription_init_best_effort(
        &g_imu_data_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        IMU_DATA_TOPIC
    );
    rclc_subscription_init_best_effort(
        &g_mag_data_sub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, MagneticField),
        MAG_DATA_TOPIC
    );
    rclc_publisher_init_best_effort(
        &g_vel_data_pub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TwistStamped),
        Ros_VelNode_VEL_DATA_TOPIC
    );
    rclc_executor_init(&g_exec, &Ros_GetSupportStruct()->context, 2, Ros_GetAllocator());
    rclc_executor_add_subscription(
        &g_exec, &g_imu_data_sub,
        &g_imu_data_msg, ImuDataCallback, ON_NEW_DATA
    );
    rclc_executor_add_subscription(
        &g_exec, &g_mag_data_sub,
        &g_mag_data_msg, MagDataCallback, ON_NEW_DATA
    );
}

void Ros_VelNode_RecordInitialHeading() {
    const __auto_type mag = Sensor_Imu_GetMag();
    g_last_heading = atan2(mag.y, mag.x);
}

void Ros_VelNode_SpinExec(const uint32_t timeout_ns) {
    const rcl_ret_t ret = rclc_executor_spin_some(&g_exec, timeout_ns);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, NODE_NAME " executor failed (code %d)", (int)ret);
    }
}

void Ros_VelNode_PublishVel() {
    const __auto_type imu = &g_imu_data_msg;
    const __auto_type mag = &g_mag_data_msg;

    const double heading = atan2(mag->magnetic_field.y, mag->magnetic_field.x);

    const double linear = ({
        const double wl = Sensor_MotorEncoder_GetLeftAngularVel();
        const double wr = Sensor_MotorEncoder_GetRightAngularVel();
        const double vl = Kinematics_AngularVelToLinearVel(wl);
        const double vr = Kinematics_AngularVelToLinearVel(wr);
        (vl + vr) / 2;
    });

    const double angular = ({
        const double dt =
            (double)Utility_GetRosTimeDiffNs(mag->header.stamp, g_last_vel_stamp) /
            1000000000.0;
        const double gyro_angular = imu->angular_velocity.z;
        const double mag_angular = (heading - g_last_heading) / dt;
        (gyro_angular * 0.5) + (mag_angular * 0.5);
    });

    g_last_vel_stamp = mag->header.stamp;
    g_last_heading = heading;

    const geometry_msgs__msg__TwistStamped msg = {
        .header.stamp = Utility_GetRosTimeStamp(),
        .header.frame_id = Utility_MakeStaticRosCString("base_link"),
        .twist.linear.x = linear,
        .twist.angular.z = angular,
    };
    const rcl_ret_t ret = rcl_publish(&g_vel_data_pub, &msg, NULL);
    if (ret != RCL_RET_OK) {
        Utility_Log(
            Utility_LogInfo, "%s failed to publish %s (code %d)",
            NODE_NAME, Ros_VelNode_VEL_DATA_TOPIC, (int)ret
        );
    }
}

static void ImuDataCallback(const void *const void_msg) {
    (void)void_msg;
}

static void MagDataCallback(const void *const void_msg) {
    (void)void_msg;
}
