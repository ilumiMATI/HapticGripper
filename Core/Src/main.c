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
#include "spi.h"
#include "tim.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// Magnetometry I2C
#include "QMC5883L.h"
#include "i2c_MA.h"
#include "i2c_sw.h"

// Joystick USB
#include "usbh_hid.h"
#include "usbh_hid_joystick.h"
#include "usbh_def.h"

//LCD
#include "ILI9341/ILI9341_STM32_Driver.h"
#include "ILI9341/ILI9341_GFX.h"

// lcd display

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
// Data structures
extern HID_JOYSTICK_Info_TypeDef DJoyStick;
extern QMC5883L_Info_TypeDef SensorDown;
extern QMC5883L_Info_TypeDef SensorUp;
extern QMC5883L_Info_TypeDef SensorGrip1;
extern QMC5883L_Info_TypeDef SensorGrip2;
extern QMC5883L_Info_TypeDef SensorGrip3;

// Motor informations
volatile int impulses = 0;
volatile double dutyCycle = 0.55;
uint8_t isDown = 1;
char isUpS, isDownS;

// Haptic informations
float vibrationForce = 0;
int force = 8500;
char isForceReached = 0;
extern char canVibrate;

// LCD informations
long lastDrawTick = 0;
char charBuforNumber[6];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
void drawNumber(uint16_t number,uint16_t x, uint16_t y, uint32_t color,uint16_t size,uint32_t bgcolor)
{
	char isSignificant = 0;
	//char index = 0;
	charBuforNumber[5] = '\n';

	charBuforNumber[0] = number / 10000;
	charBuforNumber[1] = (number%10000)/1000;
	charBuforNumber[2] = (number%1000)/100;
	charBuforNumber[3] = (number%100)/10;
	charBuforNumber[4] = number%10;

	for(uint8_t i = 0 ;i<5 ;i++)
	{
		//charBuforNumber[i] = charBuforNumber[i] == 0 ? ' ' : charBuforNumber[i] + '0';
		if(charBuforNumber[i] == 0 && !isSignificant)
		{
			charBuforNumber[i] = ' ';
		} else if(charBuforNumber != 0)
		{
			//if(isSignificant == 0) {
				isSignificant = 1;
				//index = i;
			//}
			charBuforNumber[i] += '0';
		}
	}

	ILI9341_Draw_VarText(charBuforNumber, x, y, color, size, bgcolor);
}

void SendDataLCD()
{
  for(int i = 0; i<12;i++)
  {
	  if(DJoyStick.Button[i] == 1)
	  {
		  ILI9341_Draw_Filled_Rectangle_Coord(2+i*20, 20, 18+i*20, 30, GREEN);
	  } else
	  {
		  ILI9341_Draw_Filled_Rectangle_Coord(2+i*20, 20, 18+i*20, 30, RED);
	  }
  }
  int pixelsY = (236*DJoyStick.Y/1023);
  if(pixelsY != 0)
	  ILI9341_Draw_Rectangle(2, 62, pixelsY, 5, WHITE);
  if(pixelsY != 236)
	  ILI9341_Draw_Rectangle(2+pixelsY, 62, (236-pixelsY), 5, BLACK);


  int pixelsF = (236*(255-DJoyStick.Throttle)/255);
  if(pixelsF != 0)
	  ILI9341_Draw_Rectangle(2, 91, pixelsF, 5, WHITE);
  if(pixelsF != 236)
	  ILI9341_Draw_Rectangle(2+pixelsF, 91, (236-pixelsF), 5, BLACK);
  if(HAL_GetTick() - lastDrawTick > 100) {
	  drawNumber(abs(SensorGrip1.AxisZ), 10, 120, BLACK, 2, WHITE);
	  drawNumber(abs(SensorGrip2.AxisZ), 10, 160, BLACK, 2, WHITE);
	  drawNumber(abs(SensorGrip3.AxisZ), 10, 200, BLACK, 2, WHITE);
	  lastDrawTick = HAL_GetTick();
  }
}

void InterpretJoystickData()
{
  DJoyStick.X=DJoyStick.RAW_IN[0]&0x000003FF;
  DJoyStick.Y=1023-((DJoyStick.RAW_IN[0]>>10)&0x000003FF);
  DJoyStick.Z=((DJoyStick.RAW_IN[0]>>24)&0x000000FF);
  for(int i=0;i<8;i++)
	DJoyStick.Button[i]=(char)((DJoyStick.RAW_IN[1]>>i)&0x00000001);
  DJoyStick.Throttle=	(DJoyStick.RAW_IN[1]>>8)&0x000000FF;
  for(int i=8;i<13;i++)
	DJoyStick.Button[i]=(char)((DJoyStick.RAW_IN[1]>>i+8)&0x00000001);
  DJoyStick.Hat = (uint8_t)((DJoyStick.RAW_IN[0]>>20)&0xF);
  // Mechanical functions and sensors
  force = 80 + 1920 * (255.0-DJoyStick.Throttle) / 255.0;
  //vibrationForce = DJoyStick.Throttle / 255.0;
}

void calculateControl()
{
	if(DJoyStick.Y > 550)
	  {
		  if(!DJoyStick.Button[1])
			  dutyCycle = 0.45 + 0.35 * (DJoyStick.Y - 700.0) / 323.0;
		  else
			  dutyCycle = 0.9 + 0.1 * (DJoyStick.Y - 700.0) / 323.0;
		  isDown = 1;
	  }
	  else
	  {
		  if(!DJoyStick.Button[1])
			  dutyCycle = 0.45 + 0.35 * (-(DJoyStick.Y-400.0)) / 400.0;
		  else
			  dutyCycle = 0.9 + 0.1 * (-(DJoyStick.Y-400.0)) / 400.0;
		  isDown = 0;
	  }

	  if(dutyCycle < 0.45) dutyCycle = 0.45;
	  else if(dutyCycle > 1.0) dutyCycle = 1.0;

	  QMC5883L_UpdateAxisReadings();
	  if(isDown)
		  htim1.Instance->CCR3 = (int)(MOTOR_PWM_PERIOD*(dutyCycle-0.35)) - 1;
	  else
		  htim1.Instance->CCR3 = (int)(MOTOR_PWM_PERIOD*dutyCycle) - 1;
}
void checkBounds()
{
	  if(SensorUp.RawX > 0)
	  {
		  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		  isUpS = 1;
	  }
	  if(abs(SensorDown.RawX) > 25000)
	  {
		  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		  isDownS = 1;
	  }
}

void controlMotor()
{
	if(DJoyStick.Button[0])
	  {
		  if(isDown)
		  {
			  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);

			  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			  isUpS = 0;
		  }
		  else
		  {
			  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_SET);

			  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			  isDownS = 0;
		  }
	  }
	  else
	  {
		  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
	  }
	  if(abs(SensorGrip1.AxisZ) > force ||
		 abs(SensorGrip2.AxisZ) > force ||
		 abs(SensorGrip3.AxisZ) > force)
	  {
		  isForceReached = 1;
	  }
	  else
		  isForceReached = 0;

	  if((isDown && isDownS) ||
		 (!isDown && isUpS) ||
		 (isForceReached) &&
		 isDown)
	  {
		  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
	  }
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
  //FFInit();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USB_HOST_Init();
  MX_SPI5_Init();
  /* USER CODE BEGIN 2 */

  // Motor inits
  HAL_GPIO_WritePin(MOT_IN1_GPIO_Port, MOT_IN1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MOT_IN2_GPIO_Port, MOT_IN2_Pin, GPIO_PIN_RESET);
  htim1.Instance->CCR3 = (int)(MOTOR_PWM_PERIOD*dutyCycle) - 1;
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  // sensor inits
  MAG_I2C_PinConfig();
  i2c_begin_state();
  HAL_TIM_Base_Start(&htim2);
  QMC5883L_Init();

  ILI9341_Init();
  ILI9341_Fill_Screen(BLACK);
  ILI9341_Set_Rotation(SCREEN_VERTICAL_2);
  ILI9341_Draw_Text("  Buttons  ", 0, 0, BLACK, 2, YELLOW);
  const char* numbers[12] = {"01","02","03","04","05","06","07","08","09","10","11","12"};

  for(int i=0;i<12;i++)
  {
	  ILI9341_Draw_Text(numbers[i], 5+i*20, 32, WHITE, 1, 0);
  }

  ILI9341_Draw_Text("  Axis  ", 0, 42, BLACK, 2, YELLOW);
  ILI9341_Draw_Hollow_Rectangle_Coord(0, 60, 239, 68, WHITE);
  ILI9341_Draw_Filled_Rectangle_Coord(2, 62, 237, 67, BLACK);
  ILI9341_Draw_Text("  Force  ", 0, 71, BLACK, 2, YELLOW);
  ILI9341_Draw_Hollow_Rectangle_Coord(0, 89, 239, 97, WHITE);
  ILI9341_Draw_Filled_Rectangle_Coord(2, 91, 237, 96, BLACK);
  ILI9341_Draw_Text("  Finger 1 sensor  ", 0, 100, BLACK, 2, YELLOW);
  ILI9341_Draw_Text("  Finger 2 sensor  ", 0, 140, BLACK, 2, YELLOW);
  ILI9341_Draw_Text("  Finger 3 sensor  ", 0, 180, BLACK, 2, YELLOW);

  ILI9341_Draw_Filled_Rectangle_Coord(0, 320-2*18, 240, 320, YELLOW);
  ILI9341_Draw_Text("gripper", 60, 295, BLACK, 3, YELLOW);
  ILI9341_Draw_Text("Haptic", 85, 285, BLACK, 2, YELLOW);
  //ILI9341_Draw_Vertical_Line(120, 0, 320, WHITE);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(1);
	  ReadWriteJoyStick();

	  InterpretJoystickData();
	  if(isForceReached && DJoyStick.Y > 700 && DJoyStick.Button[0])
	  {
		  canVibrate = 1;
	  }
	  else
	  {
		  canVibrate = 0;
	  }
	  Vibrate(0.8 + 0.2 * (DJoyStick.Y-700)/323);

	  SendDataLCD();

	  calculateControl();
	  checkBounds();
	  controlMotor();

    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
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
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

