#include "project/task/init.h"
#include "project/utility.h"

#include "rmw_microros/time_sync.h"

#include "cmsis_os2.h"
#include "gpio.h"

#define STATUS_LED_GPIO GPIOC
#define STATUS_LED_PIN GPIO_PIN_2

static void SetStatusLed(bool on);

void StartTimeSyncTask(void *argument) {
    (void)argument;
    Task_Init_WaitUntilDone();
    SetStatusLed(rmw_uros_epoch_synchronized());

    while (1) {
        osDelay(20000);

        const rmw_ret_t ret = rmw_uros_sync_session(1000);
        if (ret == RMW_RET_OK) {
            SetStatusLed(true);
        } else {
            SetStatusLed(false);
            Utility_Log(
                Utility_LogWarning,
                "Failed to sync time with the agent (code %d)", (int)ret
            );
        }
    }
}

static void SetStatusLed(const bool on) {
    HAL_GPIO_WritePin(
        STATUS_LED_GPIO, STATUS_LED_PIN,
        on ? GPIO_PIN_RESET : GPIO_PIN_SET
    );
}
