#include "project/sensor/battery.h"
#include "project/task/init.h"

#include "cmsis_os2.h"
#include "gpio.h"

void StartBatteryLedTask(void *arg) {
    (void)arg;
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 1);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 1);
    Task_Init_WaitUntilDone();
    while (1) {
        const double v = Sensor_Battery_GetPrevVoltage();
        if (5.5 <= v && v <= 10.0) {
            for (uint16_t i = 3; i <= 5; i++){
            HAL_GPIO_TogglePin(GPIOC, i);
            HAL_Delay(300);
            }
        osDelay(200);
    }
  }
}
