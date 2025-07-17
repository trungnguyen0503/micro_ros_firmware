#include "project/actuator/motor.h"
#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/ros/teleop_node.h"
#include "project/sensor/motor_encoder.h"
#include "project/uros/utility.h"

#include "geometry_msgs/msg/twist.h"
#include "rcl/rcl.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"
#include "rmw_microros/rmw_microros.h"
#include "sensor_msgs/msg/battery_state.h"
#include "sensor_msgs/msg/imu.h"

#include "cmsis_os2.h"
#include "usart.h"

void StartInitTask(void *argument) {
    UNUSED(argument);

    Actuator_Motor_Init();
    Sensor_MotorEncoder_Init();

    // init uros transport
    rmw_uros_set_custom_transport(
        true,
        &huart1,
        Uros_UartDmaTransport_Open,
        Uros_UartDmaTransport_Close,
        Uros_UartDmaTransport_Write,
        Uros_UartDmaTransport_Read
    );

    // init uros allocator
    Ros_allocator.allocate = Uros_Allocate;
    Ros_allocator.deallocate = Uros_Deallocate;
    Ros_allocator.reallocate = Uros_Reallocate;
    Ros_allocator.zero_allocate = Uros_ZeroAllocate;
    if (!rcutils_set_default_allocator(&Ros_allocator)) {
        // TODO:
    }

    // set global allocator and support
    rclc_support_init(&Ros_support, 0, NULL, &Ros_allocator);

    // init imu node
    rclc_node_init_default(&Ros_imu_node, "imu_node", "", &Ros_support);
    rclc_publisher_init_best_effort(
        &Ros_imu_data_pub,
        &Ros_imu_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        "imu/data"
    );

    // init battery node
    rclc_node_init_default(&Ros_battery_node, "battery_node", "", &Ros_support);
    rclc_publisher_init_best_effort(
        &Ros_battery_data_pub,
        &Ros_battery_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState),
        "battery/data"
    );

    // init teleop node
    rclc_node_init_default(&Ros_Teleop_node, "teleop_node", "", &Ros_support);
    rclc_subscription_init_best_effort(
        &Ros_Teleop_data_sub,
        &Ros_Teleop_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel"
    );
    rclc_executor_init(&Ros_Teleop_exec, &Ros_support.context, 1, &Ros_allocator);
    rclc_executor_add_subscription(
        &Ros_Teleop_exec, &Ros_Teleop_data_sub, &Ros_Teleop_msg,
        Ros_Teleop_Callback, ON_NEW_DATA
    );

    osThreadExit();
}

rclc_support_t *Task_GetRclcSupport() {
    return &Ros_support;
}

rcl_allocator_t *Task_GetRclAllocator() {
    return &Ros_allocator;
}
