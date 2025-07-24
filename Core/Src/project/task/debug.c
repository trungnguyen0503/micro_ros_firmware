#include "math.h"
#include "project/sensor/motor_encoder.h"
#include "project/uros/utility.h"

#include "cmsis_os.h"

static size_t DEBUG_min_heap_free = 0;
static size_t DEBUG_microros_min_heap_free = 0;
static double DEBUG_left_rpm = 0;
static double DEBUG_right_rpm = 0;

void StartDebugTask(void *argument) {
    UNUSED(argument);

    while (1) {
        DEBUG_min_heap_free = xPortGetMinimumEverFreeHeapSize();
        DEBUG_microros_min_heap_free = Uros_GetMinFreeHeapSize();
        DEBUG_left_rpm = Sensor_MotorEncoder_GetLeftAngularVel() * 60.0 / (2.0 * M_PI);
        DEBUG_right_rpm = Sensor_MotorEncoder_GetRightAngularVel() * 60.0 / (2.0 * M_PI);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);

        osDelay(1000);
    }
}