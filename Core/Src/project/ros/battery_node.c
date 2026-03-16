#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/sensor/battery.h"
#include "project/utility.h"

#include "std_msgs/msg/float64.h"

#include "adc.h"

static rcl_node_t g_handle = { 0 };
static rcl_publisher_t g_battery_voltage_pub = { 0 };

void Ros_BatteryNode_Init() {
    rclc_node_init_default(&g_handle, "battery_node", "robot1", Ros_GetSupportStruct());
    rclc_publisher_init_best_effort(
        &g_battery_voltage_pub,
        &g_handle,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64),
        "battery/data"
    );
}

void Ros_BatteryNode_PublishBatteryData() {
    const double voltage = Sensor_Battery_GetVoltage(5);

    const std_msgs__msg__Float64 msg = { .data = voltage };

    const rcl_ret_t ret = rcl_publish(&g_battery_voltage_pub, &msg, NULL);
    if (ret != RCL_RET_OK) {
        Utility_Log(Utility_LogWarning, "Battery publisher failed (code %d)", ret);
    }
}
