/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define	LED01_ON	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET)	// START_OUT LOW
#define	LED02_ON	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)		// START_OUT high
#define	LED03_ON	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET)		// RESET_OUT HIGH
#define	LED04_ON	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)	// RESET_OUT LOW

#define	LED01_OFF	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET)	// START_OUT LOW
#define	LED02_OFF	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)		// START_OUT high
#define	LED03_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET)		// RESET_OUT HIGH
#define	LED04_OFF	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)	// RESET_OUT LOW

#define LED01_TOGGLE   HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_1)	//1
#define LED02_TOGGLE   HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8)	//2
#define LED03_TOGGLE   HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5)	//3
#define LED04_TOGGLE   HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4)	//4

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define	BEND_INDEX 			TIM2->CCR1 = 600		//index,  PA0
#define	BEND_MIDDLE 		TIM2->CCR2 = 600		//middle, PA1
#define	BEND_RING 			TIM2->CCR3 = 600		//ring,   PA9
#define	BEND_LITTLE 		TIM2->CCR2 = 600		//little, PA4
#define BEND_THUMB_JOINT 	TIM3->CCR3 = 700
#define BEND_THUMB			TIM1->CCR4 = 600

#define	STRET_INDEX 		TIM2->CCR1 = 1000		//index,  PA0
#define	STRET_MIDDLE 		TIM2->CCR2 = 1000		//middle, PA1
#define	STRET_RING 			TIM2->CCR3 = 1000		//ring,   PA9
#define	STRET_LITTLE 		TIM2->CCR2 = 1000		//little, PA4
#define STRET_THUMB_JOINT 	TIM3->CCR3 = 1000
#define STRET_THUMB			TIM1->CCR4 = 1000

#define STROKE_MAX			1000	//robot finger max stretch
#define STROKE_MIN			750		//robot finger max bending

#define UART1_BUF_SIZE		2
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rx2_data;
uint8_t tx2_data = 125;

uint8_t rx1_data[10];
uint8_t tx1_data = 125;

uint8_t motor_id = 0;
uint8_t pris_val = 0;

uint8_t FLAG_RxCplt=1;
uint8_t FLAG_Rx1Cplt=0;

uint8_t temp_rx2_data = 0;
uint8_t temp_rx1_data = 0;
uint8_t temp_rx1_data_arr[10];
uint16_t bend_pwm_ii = 0;
uint16_t bend_thumb_pwm_ii = 0;
uint16_t stret_pwm_ii = 0;
uint8_t main_ii = 0;

uint16_t bend_test  = 0;
uint16_t thumb_test = 0;
uint16_t finger_test = 0;
float stroke_y = 0.0f;
uint16_t pwm = 0.0;

uint32_t index_motor = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void Finger_bending_stretch()
{
	TIM2->CCR1 = STROKE_MIN;		 	//index,  PA0
	HAL_Delay(200);
	TIM2->CCR2 = STROKE_MIN;		 	//middle, PA1
	HAL_Delay(200);
	TIM2->CCR3 = STROKE_MIN;		 	//ring,   PA9
	HAL_Delay(200);
	TIM3->CCR2 = STROKE_MIN;			//little, PA4
	HAL_Delay(800);

	TIM2->CCR1 = STROKE_MAX;		 	//index,  PA0
	HAL_Delay(200);
	TIM2->CCR2 = STROKE_MAX;		 	//middle, PA1
	HAL_Delay(200);
	TIM2->CCR3 = STROKE_MAX;		 	//ring,   PA9
	HAL_Delay(200);
	TIM3->CCR2 = STROKE_MAX;			//little, PA4
	HAL_Delay(800);

}

void Four_Finger_bending()
{
	TIM2->CCR1 = STROKE_MIN;		 	//index,  PA0
	TIM2->CCR2 = STROKE_MIN;		 	//middle, PA1
	TIM2->CCR3 = STROKE_MIN;		 	//ring,   PA9
	TIM3->CCR2 = STROKE_MIN;			//little, PA4
	HAL_Delay(800);

	//TIM3->CCR3 = 900;
	//TIM1->CCR4 = 850;
}

void Four_Finger_stretch()
{

	TIM2->CCR1 = STROKE_MAX;		 	//index,  PA0
	TIM2->CCR2 = STROKE_MAX;		 	//middle, PA1
	TIM2->CCR3 = STROKE_MAX;		 	//ring,   PA9
	TIM3->CCR2 = STROKE_MAX;		 	//little, PA4
	HAL_Delay(800);
	//TIM3->CCR3 = 1000;
	//TIM1->CCR4 = 1000;
}

void Full_gripping()
{

	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);

	Four_Finger_bending();
	HAL_Delay(2000);

	//STRET_THUMB_JOINT;
	//HAL_Delay(300);

	Four_Finger_stretch();
	HAL_Delay(1000);
}

void backup_longNose_grasp()
{
	int ii, jj = 0;
	TIM3->CCR3 = 650;
	TIM2->CCR1 = 800;
	for(ii = 1000; ii > 820; ii--)
	{
	  //TIM2->CCR1 = ii;
	  TIM2->CCR2 = ii;
	  HAL_Delay(15);


	}
	//TIM1->CCR4 = 950;
	HAL_Delay(5000);

	TIM2->CCR2 = 780;
	HAL_Delay(2500);

	for(jj = 0; jj < 10; jj++)
	{

	  TIM1->CCR4 = 920;
	  HAL_Delay(1500);

	  TIM1->CCR4 = 1000;
	  HAL_Delay(1500);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
	if(huart->Instance == USART2)
	{
		HAL_UART_Receive_IT(&huart2, &rx2_data, 1);
		HAL_UART_Transmit(&huart2, &rx2_data, 1, 10);

		temp_rx2_data = rx2_data;
		FLAG_RxCplt = 1;
	}

	if(huart->Instance == USART1)
	{
		HAL_UART_Receive_IT(&huart1, rx1_data, UART1_BUF_SIZE);
		//temp_rx1_data_arr = rx1_data;
		FLAG_Rx1Cplt = 1;
		motor_id = rx1_data[0];
		pris_val = rx1_data[1];
		HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
	}
}

int _write(int fd, char *str, int len)
{
	for(int i=0;i<len;i++)
	{
        //send the data to the UART1.
		HAL_UART_Transmit(&huart2, (uint8_t *)&str[i], 1, 0xFFFF);
	}
	return len;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);


  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
  LED01_TOGGLE;
  LED02_TOGGLE;
  LED03_TOGGLE;
  LED04_TOGGLE;
  HAL_Delay(200);
  LED01_TOGGLE;
  LED02_TOGGLE;
  LED03_TOGGLE;
  LED04_TOGGLE;
  HAL_Delay(200);

  //HAL_UART_Receive_IT(&huart2, &rx2_data, 1);
  HAL_UART_Receive_IT(&huart1, rx1_data, UART1_BUF_SIZE);

  TIM2->CCR1 = 1000;
  TIM2->CCR2 = 1000;
  TIM2->CCR3 = 1000;
  TIM2->CCR4 = 1000;
  TIM3->CCR2 = 1000;

  TIM3->CCR3 = 1000;
  TIM1->CCR4 = 1000;

  /* bending, stretching */
  BEND_THUMB_JOINT;
  HAL_Delay(800);

  Finger_bending_stretch();
  STRET_THUMB_JOINT;
  /* bending, stretching END*/

  bend_thumb_pwm_ii = 500;
  bend_test = 550;
  thumb_test = 900;
  finger_test = STROKE_MIN;
  stroke_y = 7;
  printf("hello world \n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //main_ii = temp_rx2_data;
	  FLAG_Rx1Cplt = 0;
	  HAL_UART_Receive_IT(&huart1, rx1_data, UART1_BUF_SIZE);
	  //HAL_UART_Transmit(&huart1, &tx1_data, 1, 10);
	  //HAL_UART_Transmit(&huart2, &tx2_data, 1, 10);

	  //HAL_Delay(200);
	 // printf("%d %d", motor_id, pris_val);

	  /* * * * * * * * * * * * * *
	   * full stretch of the finger     : 1000 pwm; which is 2.0ms.
	   * full bend of the finger(safety):  700 pwm; which is 1.4ms.
	   * motor's maximum operating 		:  500 pwm; which is 1.0ms.
	   */
	  pwm = -(pris_val * 25) + 1000;

	  switch(motor_id)
	  {
	  	  ///index finger
		  case 1:
			  TIM2->CCR1 = pwm;		 	//index,  PA0
			  //printf("%d, %d \n", pwm, pris_val);
			  //tx1_data = pwm;
			  HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
			  break;
		  /// middle finger
		  case 2:
			  TIM2->CCR2 = pwm;		 	//middle,  PA1
			  //printf("%d, %d \n", pwm, pris_val);
			  //tx1_data = pwm;
			  HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
			  break;
		  /// ring finger
		  case 3:
			  TIM2->CCR3 = pwm;		 	//ring,   PA9
			  //printf("%d, %d \n", pwm, pris_val);
			  //tx1_data = pwm;
			  HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
			  break;
		  /// little finger
		  case 4:
			  TIM3->CCR2 = pwm;		 	//little, PA4
			  //printf("%d, %d \n", pwm, pris_val);
			  //tx1_data = pwm;
			  HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
			  break;
		  case 5:
			  TIM1->CCR4 = pwm;		 	//thumb
			  //printf("%d, %d \n", pwm, pris_val);
			  //tx1_data = pwm;
			  HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
			  break;
		  case 6:
			  TIM3->CCR3 = pwm;		 	//thumb_joint
			  //printf("%d, %d \n", pwm, pris_val);
			  //tx1_data = pwm;
			  HAL_UART_Transmit(&huart1, &pris_val, 1, 10);
			  break;


	  }
/**** 20190726, PM 1:34 backup
	  switch (temp_rx1_data)
	  {
		case 49:
			TIM2->CCR1 = bend_test;		 	//index,  PA0
			TIM2->CCR2 = bend_test;		 	//middle, PA1
			//TIM2->CCR3 = bend_test;		 	//ring,   PA9
			//TIM3->CCR2 = bend_test;			//little, PA4
			HAL_Delay(1000);

			bend_thumb_pwm_ii += 50;
			TIM3->CCR3 = bend_thumb_pwm_ii;

			TIM1->CCR4 = 900; //thumb finger


			if(bend_thumb_pwm_ii > 1000)
			{
				bend_thumb_pwm_ii = 500;
				TIM3->CCR3 = bend_thumb_pwm_ii;
				HAL_Delay(1000);
			}
			HAL_Delay(1000);

			//TIM3->CCR3 = 500;
			break;

		case 50:

			TIM3->CCR3 = 620;
			TIM1->CCR4 = 1000; //thumb finger

			bend_test += 5;

			//HAL_Delay(1000);
			TIM2->CCR1 = bend_test;		 	//index,  PA0
			TIM2->CCR2 = bend_test;		 	//middle, PA1

			if(bend_test > 900)
			{
				bend_test = 800;
				TIM2->CCR1 = bend_test;		 	//index,  PA0
				TIM2->CCR2 = bend_test;		 	//middle, PA1
			}
			//HAL_Delay(1000);

			//TIM3->CCR3 = 500;
			break;

		case 51:	//3

			//TIM3->CCR3 = 620;
			TIM3->CCR3 = 550;
			//TIM1->CCR4 = 900; //thumb finger
			bend_test = 750;

			TIM2->CCR1 = bend_test;		 	//index,  PA0
			TIM2->CCR2 = bend_test;		 	//middle, PA1
			TIM2->CCR2 = bend_test;		 	//middle, PA1
			TIM2->CCR3 = bend_test;		 	//ring,   PA9
			TIM3->CCR2 = bend_test;			//little, PA4

			HAL_Delay(1000);

			if(thumb_test < 850)
			{
				thumb_test = 900;
				TIM1->CCR4 = thumb_test; //thumb finger
				HAL_Delay(1000);
			}

			thumb_test = thumb_test - 10;
			TIM1->CCR4 = thumb_test;
			printf("%d \n", thumb_test);
			break;

		case 52:	//4 stroke mm -> pwm test
			pwm = (stroke_y * 25) + 500;

			TIM2->CCR1 = pwm;		 	//index,  PA0
			printf("%f, %f \n", pwm, stroke_y);
			//HAL_Delay(100);

			stroke_y = stroke_y + 0.1f;

			if(stroke_y > 20)
			{
				stroke_y = 7;
				TIM2->CCR1 = pwm;
				HAL_Delay(1000);
			}

			//HAL_Delay(100);
			break;



		case 50000:	//물체잡기 시연, 50
			LED02_ON;
			HAL_Delay(200);
			LED02_OFF;
			HAL_Delay(200);
			//TIM3->CCR3 = 600;
			BEND_THUMB_JOINT;
			HAL_Delay(200);

			for(bend_pwm_ii = 1000; bend_pwm_ii > 730; bend_pwm_ii--)
			{

				TIM2->CCR1 = bend_pwm_ii;
				TIM2->CCR2 = bend_pwm_ii;
				TIM2->CCR3 = bend_pwm_ii;
				TIM3->CCR2 = bend_pwm_ii;

				HAL_Delay(5);

			}
			temp_rx1_data = 0;
			break;

		case 511111: //물체잡기 시연, 51
			LED03_ON;
			HAL_Delay(200);
			LED03_OFF;
			HAL_Delay(200);

			for(stret_pwm_ii = bend_pwm_ii; stret_pwm_ii < 1000; stret_pwm_ii++)
			{

				TIM2->CCR1 = stret_pwm_ii;
				TIM2->CCR2 = stret_pwm_ii;
				TIM2->CCR3 = stret_pwm_ii;
				TIM3->CCR2 = stret_pwm_ii;

				HAL_Delay(5);
			}
			temp_rx1_data = 0;
			STRET_THUMB_JOINT;

			break;
		case 4:
			LED04_ON;
			HAL_Delay(200);
			LED04_OFF;
			HAL_Delay(200);
			break;
		default:
			LED01_OFF;
			LED02_OFF;
			LED03_OFF;
			LED04_OFF;
			break;
	}
***/

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
