#include "project/sensor/battery.h"
#include "project/task/init.h"

#include "cmsis_os2.h"
#include "gpio.h"

void StartBatteryLedTask(void *arg) {
    (void)arg;
    Task_Init_WaitUntilDone();
    while (1) {
        const double v = Sensor_Battery_GetPrevVoltage();
        if (5.5 <= v && v <= 9.0) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
        } else {
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 1);
        }
        osDelay(500);
    }
}
