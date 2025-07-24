#include "rmw_microros/time_sync.h"

#include "cmsis_os2.h"
#include "gpio.h"

void StartTimeSyncTask(void *argument) {
    (void)argument;

    while (1) {
        HAL_GPIO_WritePin(
            GPIOC, GPIO_PIN_2,
            rmw_uros_epoch_synchronized() ? GPIO_PIN_SET : GPIO_PIN_RESET
        );

        osDelay(60000);

        const rmw_ret_t ret = rmw_uros_sync_session(100);
        if (ret != RMW_RET_OK) {
            // TODO:
        }
    }
}
