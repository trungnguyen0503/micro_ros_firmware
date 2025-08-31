#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/sensor/battery.h"
#include "project/utility.h"

#include "sensor_msgs/msg/battery_state.h"

#include "adc.h"

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

void Ros_BatteryNode_PublishBatteryData() {
    const double voltage = Sensor_Battery_GetVoltage(5);

    const sensor_msgs__msg__BatteryState msg = {
        .header.stamp = Utility_GetRosTimeStamp(),
        .voltage = (float)voltage,
        .percentage = (float)(voltage / Sensor_Battery_MAX_VOLTAGE * 100.0),
        .present = true,
        .power_supply_status = sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_DISCHARGING,
    };

    const rcl_ret_t ret = rcl_publish(Ros_BatteryNode_GetBatteryDataPub(), &msg, NULL);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "Battery publisher failed (code %d)", ret);
    }
}
