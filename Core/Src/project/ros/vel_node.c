#include "project/ros/vel_node.h"
#include "project/actuator/motor.h"
#include "project/kinematics.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/sensor/imu.h"
#include "project/sensor/motor_encoder.h"
#include "project/utility.h"

#include "geometry_msgs/msg/twist_stamped.h"
#include "rclc/node.h"
#include "rclc/publisher.h"
#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/magnetic_field.h"

#include <math.h>

#define NODE_NAME "vel_encoder_node"

enum {
    FRAME_ID_CAPACITY = 16
};

static rcl_node_t g_node = { 0 };
static rcl_publisher_t g_vel_data_pub = { 0 };

static double g_v_max = 0;

void Ros_VelNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "", Ros_GetSupportStruct());
    rclc_publisher_init_best_effort(
        &g_vel_data_pub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TwistStamped),
        Ros_VelNode_VEL_DATA_TOPIC
    );

    g_v_max = Kine_AngularVelToLinearVel(Actuator_Motor_MAX_ANGULAR_VEL);
}

void Ros_VelNode_PublishVel() {
    const double wl = Sensor_MotorEncoder_GetLeftAngularVel();
    const double wr = Sensor_MotorEncoder_GetRightAngularVel();
    double vl = Kine_AngularVelToLinearVel(wl);
    if (fabs(vl) > g_v_max) {
        vl = 0;
    }
    double vr = Kine_AngularVelToLinearVel(wr);
    if (fabs(vr) > g_v_max) {
        vr = 0;
    }
    const double linear = (vl + vr) / 2;
    const double angular = (vr - vl) / Kine_WHEEL_BASE;

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
