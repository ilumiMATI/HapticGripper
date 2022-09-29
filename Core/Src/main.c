/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "QMC5883L.h"
#include "i2c_MA.h"
#include "i2c_sw.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOTOR_PWM_PERIOD 20000
//#define DEBUG_MAG // test if sensors work
//#define DEBUG_MOT // show motor dir
#define DEBUG_BOUNDS // check if boundaries are properly configured
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile int impulses = 0;
volatile uint8_t prevState = 0;
volatile uint8_t isDown = 1;
volatile double dutyCycle = 0.75;
volatile int force = 8500;
char nack = 60;
extern QMC5883L_Info_TypeDef SensorDown;
extern QMC5883L_Info_TypeDef SensorUp;
extern QMC5883L_Info_TypeDef SensorGrip1;
extern QMC5883L_Info_TypeDef SensorGrip2;
extern QMC5883L_Info_TypeDef SensorGrip3;
char isUpS, isDownS;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  //QMC5583L_SetHandle(&hi2c1);
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // Motor inits
  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
  htim1.Instance->CCR3 = (int)(MOTOR_PWM_PERIOD*dutyCycle) - 1;
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  // sensor inits
//  if(HAL_I2C_IsDeviceReady(&hi2c1, 0x1A, 10, 100) == HAL_OK)
//  	  QMC5883L_Init();
  //GPIOG->AFR[0] = 0;
  //GPIOG->AFR[1] = 0;
  //GPIOB->MODER |= GPIO_MODER_OUTPUT << (8*2);
  //GPIOB->OTYPER |= GPIO_OTYPER_PP << (8);
  //GPIOB->OSPEEDR |= GPIO_OSPEEDR_LOW << (8*2);
  //GPIOB->PUPDR |= GPIO_PUPDR_PULL_UP << (8*2);
  //GPIOB->BSRR = MAG_SCL_Pin << 16;
  //GPIOB->BSRR = MAG_SCL_Pin; // 1 SCL
  SW_I2C_initial();
  i2c_port_initial(SW_I2C1);
  HAL_TIM_Base_Start(&htim2);
  QMC5883L_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  HAL_Delay(10);
//	  if(QMC5583L_IsReady() == 0x01)
//	  {
//	     QMC5883L_UpdateAxisReadings();
//	  }
	  //QMC5883L_UpdateAxisReadings();
	  nack = SW_I2C_ReadControl_8Bit(SW_I2C1,QMC5883L_ADDR,QMC5883L_CTRL);
	  QMC5883L_UpdateAxisReadings();
	  if(isDown)
		  htim1.Instance->CCR3 = (int)(MOTOR_PWM_PERIOD*(dutyCycle-0.35)) - 1;
	  else
		  htim1.Instance->CCR3 = (int)(MOTOR_PWM_PERIOD*dutyCycle) - 1;

#ifdef DEBUG_MAGZ

	  if((SensorGrip1.AxisZ > 1000) | (SensorGrip2.AxisZ > 1000) | (SensorGrip3.AxisZ > 1000))
	  {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	  }
	  if((abs(SensorUp.AxisZ) > 1000) | (abs(SensorDown.AxisZ) > 1000))
	  {
		  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
	  }
#endif
#ifdef DEBUG_BOUNDS
	  if(abs(SensorUp.RawX) > 28000 )
	  {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		  isUpS = 1;
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		  isUpS = 0;
	  }
	  if(abs(SensorDown.RawX) > 28000 && abs(SensorDown.RawX) < 30000 && SensorDown.RawZ < 1500)
	  {
		  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		  isDownS = 1;
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		  isDownS = 0;
	  }

#endif
//	  if(isDown && isDownS)
//	  {
//		  isDown = 0;
//		  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
//		  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_SET);
//	  }
//	  if(!isDown && isUpS)
//	  {
//		  isDown = 1;
//		  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_SET);
//		  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
//	  }
	  HAL_Delay(1);
	  if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET)  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET)
	  {
#ifdef DEBUG_MOT
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
#endif
		  if(prevState != 1)
		  {
			  isDown = !isDown;

			  if(isDown)
			  {
#ifdef DEBUG_MOT
				  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
#endif
				  // dół
				  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
			  }
			  else
			  {
#ifdef DEBUG_MOT
				  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
#endif
				  // góra
				  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_SET);
			  }
		  }
		  prevState = 1;
	  }
	  if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
	  {
#ifdef DEBUG_MOT
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
#endif
		  // stop
		  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);

		  prevState = 0;
	  }
	  if((isDown && isDownS) ||
		 (!isDown && isUpS) ||
		 (abs(SensorGrip1.AxisZ) > force ||
		 abs(SensorGrip2.AxisZ) > force ||
		 abs(SensorGrip3.AxisZ) > force) &&
		 isDown)
	  {
		  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
	  }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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

