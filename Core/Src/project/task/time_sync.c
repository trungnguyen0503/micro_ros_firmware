#include "rmw_microros/time_sync.h"

#include "cmsis_os2.h"

void StartTimeSyncTask(void *argument) {
    (void)argument;
    while (1) {
        osDelay(60000);
        const rmw_ret_t ret = rmw_uros_sync_session(100);
        if (ret != RMW_RET_OK) {
            // TODO:
        }
    }
}