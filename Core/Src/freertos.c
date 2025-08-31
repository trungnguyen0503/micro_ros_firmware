/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* USER CODE END Variables */
/* Definitions for InitTask */
osThreadId_t InitTaskHandle;
const osThreadAttr_t InitTask_attributes = {
  .name = "InitTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime7,
};
/* Definitions for DebugTask */
osThreadId_t DebugTaskHandle;
const osThreadAttr_t DebugTask_attributes = {
  .name = "DebugTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for IMUTask */
osThreadId_t IMUTaskHandle;
const osThreadAttr_t IMUTask_attributes = {
  .name = "IMUTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BatteryTask */
osThreadId_t BatteryTaskHandle;
const osThreadAttr_t BatteryTask_attributes = {
  .name = "BatteryTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TimeSyncTask */
osThreadId_t TimeSyncTaskHandle;
const osThreadAttr_t TimeSyncTask_attributes = {
  .name = "TimeSyncTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for OdometryTask */
osThreadId_t OdometryTaskHandle;
const osThreadAttr_t OdometryTask_attributes = {
  .name = "OdometryTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DiffDriveTask */
osThreadId_t DiffDriveTaskHandle;
const osThreadAttr_t DiffDriveTask_attributes = {
  .name = "DiffDriveTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for VelocityTask */
osThreadId_t VelocityTaskHandle;
const osThreadAttr_t VelocityTask_attributes = {
  .name = "VelocityTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BatteryLedTask */
osThreadId_t BatteryLedTaskHandle;
const osThreadAttr_t BatteryLedTask_attributes = {
  .name = "BatteryLedTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartInitTask(void *argument);
void StartDebugTask(void *argument);
void StartIMUTask(void *argument);
void StartBatteryTask(void *argument);
void StartTimeSyncTask(void *argument);
void StartOdometryTask(void *argument);
void StartDiffDriveTask(void *argument);
void StartVelocityTask(void *argument);
void StartBatteryLedTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= 1;
    DWT->CYCCNT = 0;
}

__weak unsigned long getRunTimeCounterValue(void) {
    return DWT->CYCCNT;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of InitTask */
  InitTaskHandle = osThreadNew(StartInitTask, NULL, &InitTask_attributes);

  /* creation of DebugTask */
  DebugTaskHandle = osThreadNew(StartDebugTask, NULL, &DebugTask_attributes);

  /* creation of IMUTask */
  IMUTaskHandle = osThreadNew(StartIMUTask, NULL, &IMUTask_attributes);

  /* creation of BatteryTask */
  BatteryTaskHandle = osThreadNew(StartBatteryTask, NULL, &BatteryTask_attributes);

  /* creation of TimeSyncTask */
  TimeSyncTaskHandle = osThreadNew(StartTimeSyncTask, NULL, &TimeSyncTask_attributes);

  /* creation of OdometryTask */
  OdometryTaskHandle = osThreadNew(StartOdometryTask, NULL, &OdometryTask_attributes);

  /* creation of DiffDriveTask */
  DiffDriveTaskHandle = osThreadNew(StartDiffDriveTask, NULL, &DiffDriveTask_attributes);

  /* creation of VelocityTask */
  VelocityTaskHandle = osThreadNew(StartVelocityTask, NULL, &VelocityTask_attributes);

  /* creation of BatteryLedTask */
  BatteryLedTaskHandle = osThreadNew(StartBatteryLedTask, NULL, &BatteryLedTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartInitTask */
/**
 * @brief  Function implementing the InitTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartInitTask */
__weak void StartInitTask(void *argument)
{
  /* USER CODE BEGIN StartInitTask */
  UNUSED(argument);
  /* USER CODE END StartInitTask */
}

/* USER CODE BEGIN Header_StartDebugTask */
/**
* @brief Function implementing the DebugTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugTask */
__weak void StartDebugTask(void *argument)
{
  /* USER CODE BEGIN StartDebugTask */
  UNUSED(argument);
  /* USER CODE END StartDebugTask */
}

/* USER CODE BEGIN Header_StartIMUTask */
/**
 * @brief Function implementing the IMUTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartIMUTask */
__weak void StartIMUTask(void *argument)
{
  /* USER CODE BEGIN StartIMUTask */
  UNUSED(argument);
  /* USER CODE END StartIMUTask */
}

/* USER CODE BEGIN Header_StartBatteryTask */
/**
 * @brief Function implementing the BatteryTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartBatteryTask */
__weak void StartBatteryTask(void *argument)
{
  /* USER CODE BEGIN StartBatteryTask */
  UNUSED(argument);
  /* USER CODE END StartBatteryTask */
}

/* USER CODE BEGIN Header_StartTimeSyncTask */
/**
* @brief Function implementing the TimeSyncTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTimeSyncTask */
__weak void StartTimeSyncTask(void *argument)
{
  /* USER CODE BEGIN StartTimeSyncTask */
  UNUSED(argument);
  /* USER CODE END StartTimeSyncTask */
}

/* USER CODE BEGIN Header_StartOdometryTask */
/**
* @brief Function implementing the OdometryTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOdometryTask */
__weak void StartOdometryTask(void *argument)
{
  /* USER CODE BEGIN StartOdometryTask */
  UNUSED(argument);
  /* USER CODE END StartOdometryTask */
}

/* USER CODE BEGIN Header_StartDiffDriveTask */
/**
* @brief Function implementing the DiffDriveTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDiffDriveTask */
__weak void StartDiffDriveTask(void *argument)
{
  /* USER CODE BEGIN StartDiffDriveTask */
  UNUSED(argument);
  /* USER CODE END StartDiffDriveTask */
}

/* USER CODE BEGIN Header_StartVelocityTask */
/**
* @brief Function implementing the Velocity thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartVelocityTask */
__weak void StartVelocityTask(void *argument)
{
  /* USER CODE BEGIN StartVelocityTask */
  UNUSED(argument);
  /* USER CODE END StartVelocityTask */
}

/* USER CODE BEGIN Header_StartBatteryLedTask */
/**
* @brief Function implementing the BatteryLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBatteryLedTask */
__weak void StartBatteryLedTask(void *argument)
{
  /* USER CODE BEGIN StartBatteryLedTask */
  UNUSED(argument);
  /* USER CODE END StartBatteryLedTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

