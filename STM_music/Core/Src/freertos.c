/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Audio.h"


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
char buffer[4];
bool stop = 0;
extern volatile uint8_t audio_volume;
/* USER CODE END Variables */
/* Definitions for musicTask */
osThreadId_t musicTaskHandle;
const osThreadAttr_t musicTask_attributes = {
  .name = "musicTask",
  .stack_size = 2560 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for controlTask */
osThreadId_t controlTaskHandle;
const osThreadAttr_t controlTask_attributes = {
  .name = "controlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for uartRxTask */
osThreadId_t uartRxTaskHandle;
const osThreadAttr_t uartRxTask_attributes = {
  .name = "uartRxTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void InitializePlayDirectory(PlayDirectoryFunction *fcn){
	//play_directory = fcn;
}
/* USER CODE END FunctionPrototypes */

void StartMusicTask(void *argument);
void StartControlTask(void *argument);
void StartUartRxTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
}
/* USER CODE END 4 */

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
//  SemaphoreHandle_t xPlayPauseSemaphore;
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of musicTask */
  musicTaskHandle = osThreadNew(StartMusicTask, NULL, &musicTask_attributes);

  /* creation of controlTask */
  controlTaskHandle = osThreadNew(StartControlTask, NULL, &controlTask_attributes);

  /* creation of uartRxTask */
  uartRxTaskHandle = osThreadNew(StartUartRxTask, NULL, &uartRxTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMusicTask */
/**
  * @brief  Function implementing the musicTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMusicTask */
void StartMusicTask(void *argument)
{
  /* USER CODE BEGIN StartMusicTask */
  /* Infinite loop */
	  init_sd();
	  for(;;){
		  play_directory("", 0);
		  osDelay(20);
	  }
  /* USER CODE END StartMusicTask */
}

/* USER CODE BEGIN Header_StartControlTask */
/**
* @brief Function implementing the controlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartControlTask */
void StartControlTask(void *argument)
{
  /* USER CODE BEGIN StartControlTask */
  /* Infinite loop */
	static volatile uint8_t volume = 50;
	audio_volume = volume;
	htim2.Instance->CNT = volume;
	SetAudioVolume((audio_volume*255)/100);
  for(;;)
  {
	  if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)){
	  		  HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
	  		  stop = !stop;
	  }
	  if(stop == 1){
		  vTaskSuspend(musicTaskHandle);
	  }
	  else{
		  vTaskResume(musicTaskHandle);
	  }

	  if(volume != htim2.Instance->CNT){
//		  if(htim2.Instance->CNT >= 1 && htim2.Instance->CNT <= 98)
//		  {}
//		  else htim2.Instance->CNT = volume;
		  audio_volume = htim2.Instance->CNT;
		  volume = htim2.Instance->CNT;
		  SetAudioVolume((audio_volume*255)/100);
		  snprintf(buffer, 4 , "?%2d", audio_volume);
		  myprintf(buffer,4);
		  volume = htim2.Instance->CNT;
		  audio_volume = htim2.Instance->CNT;
	  }
	  osDelay(10);
  }
  /* USER CODE END StartControlTask */
}

/* USER CODE BEGIN Header_StartUartRxTask */
/**
* @brief Function implementing the uartRxTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartRxTask */
void StartUartRxTask(void *argument)
{
  /* USER CODE BEGIN StartUartRxTask */
  /* Infinite loop */
  for(;;)
  {
	  recieveData();
    osDelay(1);
  }
  /* USER CODE END StartUartRxTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

