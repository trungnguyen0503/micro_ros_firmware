#include "project/actuator/motor.h"
#include "project/ros/battery_node.h"
#include "project/ros/global.h"
#include "project/ros/imu_node.h"
#include "project/sensor/motor_encoder.h"
#include "project/uros/utility.h"

#include "rcl/rcl.h"
#include "rclc/executor.h"
#include "rclc/rclc.h"
#include "rmw_microros/rmw_microros.h"
#include "sensor_msgs/msg/battery_state.h"
#include "sensor_msgs/msg/imu.h"
#include "std_msgs/msg/int32.h"

#include "cmsis_os2.h"
#include "usart.h"

static rcl_node_t g_test_node = { 0 };
static rcl_subscription_t g_test_sub = { 0 };
static rclc_executor_t g_executor = { 0 };

static std_msgs__msg__Int32 DEBUG_int = { 0 };

static uint32_t g_sub_callback_count = 0;

static void TestSubscriptionCallback(const void *void_msg);

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

    // init test node
    rclc_node_init_default(&g_test_node, "test_node", "", &Ros_support);
    rclc_subscription_init_best_effort(
        &g_test_sub,
        &g_test_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "pi_test"
    );
    rclc_executor_init(&g_executor, &Ros_support.context, 1, &Ros_allocator);
    rclc_executor_add_subscription(
        &g_executor, &g_test_sub, &DEBUG_int, TestSubscriptionCallback, ON_NEW_DATA
    );

    osThreadExit();
}

void StartSubscribeTask(void *argument) {
    UNUSED(argument);

    const uint32_t interval_ms = 100;
    const uint32_t interval_ns = interval_ms * 1000;

    while (1) {
        rclc_executor_spin_some(&g_executor, interval_ns);
        osDelay(interval_ms);
    }
}

static void TestSubscriptionCallback(const void *const void_msg) {
    UNUSED(void_msg);
    g_sub_callback_count++;
}

rclc_support_t *Task_GetRclcSupport() {
    return &Ros_support;
}

rcl_allocator_t *Task_GetRclAllocator() {
    return &Ros_allocator;
}
