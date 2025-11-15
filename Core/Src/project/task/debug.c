#include "math.h"
#include "project/sensor/motor_encoder.h"
#include "project/uros/utility.h"

#include "cmsis_os.h"

static size_t DEBUG_freertos_min_heap_free = 0;
static size_t DEBUG_microros_min_heap_free = 0;

void StartDebugTask(void *argument) {
    UNUSED(argument);

    while (1) {
        DEBUG_freertos_min_heap_free = xPortGetMinimumEverFreeHeapSize();
        DEBUG_microros_min_heap_free = Uros_GetMinFreeHeapSize();
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);

        osDelay(1000);
    }
}