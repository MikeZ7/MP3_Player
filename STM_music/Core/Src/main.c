/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "fatfs.h"
#include "i2s.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h> //for va_list var arg functions


#include "Audio.h"
#include "mp3dec.h"
#include "mp3.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char RecieveBuffer[2] = "--";
extern bool stop;
int mp3_buffer_counter = 0;
volatile uint8_t audio_volume = 50;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#include "mp3.h"
#include <math.h>

#define AUDIO_I2C_ADDR	0x94
#define BUFFER_SIZE		4096

static uint8_t audio_data[2 * BUFFER_SIZE];

//some variables for FatFs
FATFS FatFs; 	//Fatfs handle
FRESULT fres; //Result after operations
FIL file;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//some variables for FatFs

volatile int			    enum_done = 0;
static volatile uint8_t     audio_is_playing = 0;
extern volatile uint16_t volume;
/* just for test */
extern float                cur_ratio;
char playing_title[34];
uint8_t stop_mp3 = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void init_sd(){
	//myprintf("\r\n~ SD card demo by kiwih ~\r\n\r\n");

	  HAL_Delay(1000); //a short delay is important to let the SD card settle



	  //Open the file system
	  fres = f_mount(&FatFs, "", 1); //1=mount now
	  if (fres != FR_OK) {
		myprintf("f_mount error (%i)\r\n", fres);
		while(1);
	  }
}

void deinit_sd(){
	  f_mount(NULL, "", 0);
}

static void AudioCallback(void) {
    audio_is_playing = 0;
}

static uint32_t fd_fetch(void *parameter, uint8_t *buffer, uint32_t length) {
	uint32_t read_bytes = 0;

	f_read((FIL *)parameter, (void *)audio_data, length, (UINT*)&read_bytes);
	memcpy(buffer, audio_data, read_bytes);
	    if (read_bytes <= 0) return 0;

	    return read_bytes;
}
static uint32_t mp3_callback(MP3FrameInfo *header,
                             int16_t *buffer,
                             uint32_t length) {

	while (audio_is_playing == 1);
    audio_is_playing = 1;
	ProvideAudioBuffer(buffer, length);

    return 0;
}

void play_mp3(char* filename) {
    struct mp3_decoder *decoder;

	if (FR_OK == f_open(&file, filename, FA_OPEN_EXISTING | FA_READ)) {
		/* Play mp3 */
		InitializeAudio(Audio44100HzSettings);
		SetAudioVolume(0xCF);
		PlayAudioWithCallback(AudioCallback);

        decoder = mp3_decoder_create();
        if (decoder != NULL) {

            decoder->fetch_data         = fd_fetch;
            decoder->fetch_parameter    = (void *)&file;
            decoder->output_cb          = mp3_callback;

            while (mp3_decoder_run_pvc(decoder) != -1 && stop_mp3 != 1);
            stop_mp3 = 0;
            mp3_decoder_delete(decoder);
            mp3_buffer_counter = 0;
        }

        /* Re-initialize and set volume to avoid noise */
        InitializeAudio(Audio44100HzSettings);
        SetAudioVolume(0);

        /* reset the playing flag */
        audio_is_playing = 0;

        /* Close currently open file */
        f_close(&file);
    }
}


/*
 * play directory
 */
static const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

FRESULT play_directory (const char* path, unsigned char seek) {
	FRESULT     res;
	FILINFO     fno;
	DIR         dir;

    /* This function is assuming non-Unicode cfg. */
	char        *fn;
	char        buffer[200];

	res = f_opendir(&dir, path);
	if (res == FR_OK) {
		for (;;) {

			res = f_readdir(&dir, &fno);

			if (res != FR_OK || fno.fname[0] == 0) break; //break if empty

			if (fno.fname[0] == '.') continue;//found a file

			fn = fno.fname;
//        #endif
			//check if directory
			if (fno.fattrib & AM_DIR) {

			} else {
                /* It is a file. */
				sprintf(buffer,"                                \0");
				sprintf(buffer, "%s/%s", path, fn);

				if (strcmp("mp3", get_filename_ext(buffer)) == 0) {

					if (seek) {
						seek--;
						continue;
					}
					snprintf(playing_title,17, "%s", buffer+1);
					snprintf(playing_title+17,17, "%s", buffer+17);
					myprintf("%s!", playing_title);
					HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
					play_mp3(buffer);
				}
			}
		}
	}

	return res;
}

void myprintf(const char *fmt, ...) {
  static char buffer[512];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  int len = strlen(buffer);
  HAL_UART_Transmit_DMA(&huart2, (uint8_t*)buffer, len);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* FPU initialization */
  	//SCB->CPACR |= ((3 << 10*2)|(3 << 11*2));
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S3_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_FATFS_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  InitializePlayDirectory(play_directory);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void transmitData(){
	HAL_UART_Transmit_DMA(&huart2, playing_title, 1); //unused
}
void recieveData(){
	HAL_UART_Receive_DMA(&huart2, RecieveBuffer, 2);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART2){
		HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
		  if(RecieveBuffer[0] == 'v'){
			  if(RecieveBuffer[1] == '-' && audio_volume>11){
				  audio_volume-=10;
				  SetAudioVolume((audio_volume*255)/100);
			  }
			  else if(RecieveBuffer[1] == '+' && audio_volume<246){
				  audio_volume+=10;
				  SetAudioVolume((audio_volume*255)/100);
			  }
		  }
		  else if(RecieveBuffer[0] == 's'){
			  stop=!stop;
		  }
		  else{
			  myprintf("No such command!\n\r");
		  }
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART2){
		HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
	}
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
