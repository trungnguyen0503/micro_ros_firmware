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

#include <math.h>

#define NODE_NAME "vel_encoder_node"

enum {
    FRAME_ID_CAPACITY = 16
};

static rcl_node_t g_node = { 0 };
static rcl_publisher_t g_vel_data_pub = { 0 };

static double linear = 0;
static double angular = 0;

#define V_MAX Kine_AngularVelToLinearVel(Actuator_Motor_MAX_ANGULAR_VEL)
#define V_MIN Kine_AngularVelToLinearVel(Actuator_Motor_MIN_ANGULAR_VEL)

void Ros_VelNode_Init() {
    rclc_node_init_default(&g_node, NODE_NAME, "robot2", Ros_GetSupportStruct());
    rclc_publisher_init_best_effort(
        &g_vel_data_pub, &g_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, TwistStamped),
        Ros_VelNode_VEL_DATA_TOPIC
    );
}

void Ros_VelNode_PublishVel() {
    const double wl = Sensor_MotorEncoder_GetLeftAngularVel();
    const double wr = Sensor_MotorEncoder_GetRightAngularVel();
    double vl = Kine_AngularVelToLinearVel(wl);
    if (fabs(vl) > V_MAX || fabs(vl) < V_MIN) {
        vl = 0;
    }
    double vr = Kine_AngularVelToLinearVel(wr);
    if (fabs(vr) > V_MAX || fabs(vr) < V_MIN) {
        vr = 0;
    }
    linear = (vl + vr) / 2;
    if (fabs(linear) < 0.009) {
            linear = 0;
        }

    angular = (vr - vl) / Kine_WHEEL_BASE;
    if (fabs(angular) < 0.08) {
            angular = 0;
        }
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
