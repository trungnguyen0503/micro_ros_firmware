#include "project/ros/battery_node.h"
#include "project/ros/global.h"

#include "sensor_msgs/msg/battery_state.h"

static rcl_node_t Ros_BatteryNode_handle = { 0 };
static rcl_publisher_t Ros_BatteryNode_battery_data_pub = { 0 };

void Ros_BatteryNode_Init() {
    rclc_node_init_default(&Ros_BatteryNode_handle, "battery_node", "", Ros_GetSupportStruct());
    rclc_publisher_init_best_effort(
        &Ros_BatteryNode_battery_data_pub,
        &Ros_BatteryNode_handle,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState),
        "battery/data"
    );
}

const rcl_node_t *Ros_BatteryNode_GetHandle() {
    return &Ros_BatteryNode_handle;
}

const rcl_publisher_t *Ros_BatteryNode_GetBatteryDataPub() {
    return &Ros_BatteryNode_battery_data_pub;
}
