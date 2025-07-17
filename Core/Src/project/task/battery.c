#include "project/ros/battery_node.h"

#include "rcl/rcl.h"
#include "sensor_msgs/msg/battery_state.h"

#include "adc.h"
#include "cmsis_os.h"

void StartBatteryTask(void *argument) {
    UNUSED(argument);

    while (1) {
        const uint32_t adc_value_max = 4095;
        const float adc_voltage_ref = 3.3F;
        const float adc_scale = 10;
        const float battery_voltage_max = 12.6F;

        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 5);
        const uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);

        const float voltage = (float)adc_value / (float)adc_value_max * adc_voltage_ref * adc_scale;
        const sensor_msgs__msg__BatteryState msg = {
            .header.stamp.sec = (int32_t)(osKernelGetTickCount() / 1000),
            .voltage = voltage,
            .percentage = voltage / battery_voltage_max * 100.0F,
            .present = true,
            .power_supply_status = sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_DISCHARGING,
        };

        const rcl_ret_t ret = rcl_publish(&Ros_Battery_data_pub, &msg, NULL);
        if (ret != RCL_RET_OK) {
        }

        osDelay(10000);
    }
}