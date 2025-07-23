#include "project/ros/battery_node.h"
#include "project/ros/global.h"

#include "sensor_msgs/msg/battery_state.h"

rcl_node_t Ros_Battery_node = { 0 };
rcl_publisher_t Ros_Battery_data_pub = { 0 };

void Ros_Battery_InitNode() {
    rclc_node_init_default(&Ros_Battery_node, "battery_node", "", &Ros_support);
    rclc_publisher_init_best_effort(
        &Ros_Battery_data_pub,
        &Ros_Battery_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState),
        "battery/data"
    );
}
