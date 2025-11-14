#include "project/sensor/battery.h"
#include "project/task/init.h"

#include "cmsis_os2.h"
#include "gpio.h"

#define V_LOW_BATTERY_MIN 5.5
#define V_LOW_BATTERY_MAX 10.5

void StartBatteryLedTask(void *arg) {
    (void)arg;
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 1);
    Task_Init_WaitUntilDone();
    while (1) {
        const double v = Sensor_Battery_GetPrevVoltage();
        if (V_LOW_BATTERY_MIN <= v && v <= V_LOW_BATTERY_MAX) {
            for (uint16_t i = 3; i <= 5; i++){
                HAL_GPIO_TogglePin(GPIOC, i);
                osDelay(300);
            }
            osDelay(200);
        }
    }
}
