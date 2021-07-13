/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "SML_Factory.h"
#include "OBISMapping.h"
#include "LMN_ErrorMsg.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RS485_3_WE_PIN GPIO_PIN_8
#define RS485_3_WE_PORT GPIOC
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define TRACE(_s, ...) do{ printf("[Demo][%s]<%u> ", __FUNCTION__, __LINE__); printf(_s, ##__VA_ARGS__); printf("\r\n");} while(0)

#define GRENZWERT_Spannung 230
#define GRENZWERT_Strom 5
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 5
};
/* Definitions for toggleLEDTask */
osThreadId_t toggleLEDTaskHandle;
const osThreadAttr_t toggleLEDTask_attributes = {
  .name = "toggleLEDTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
void StartDefaultTask(void *argument);
void StarttoggleLEDTask(void *argument);

uint64_t get_Value(ParamID_t messValue);
uint64_t Kleinste(uint64_t Spannung[]);
uint64_t Grosste(uint64_t Spannung[]);
void Umschaltung(uint64_t kleinste, uint64_t grosste);
uint64_t grenzWert(uint64_t arr[]);

static int Output1 = 0;
static int Output2 = 0;
static int Output3 = 0;

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
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(GPIOB, Out1L1_Pin, GPIO_PIN_SET);
  Output1 = 1;
  HAL_GPIO_WritePin(GPIOB, Out2L2_Pin, GPIO_PIN_SET);
  Output2 = 2;
  HAL_GPIO_WritePin(GPIOB, Out3L3_Pin, GPIO_PIN_SET);
  Output3 = 3;
  HAL_Delay(5000);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of toggleLEDTask */
  toggleLEDTaskHandle = osThreadNew(StarttoggleLEDTask, NULL, &toggleLEDTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, Out2L1_Pin|Out2L2_Pin|Out2L3_Pin|Out3L1_Pin
                            |Out3L2_Pin|Out3L3_Pin|Out1L1_Pin|Out1L2_Pin
                            |Out1L3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RS485_3_WE_GPIO_Port, RS485_3_WE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD4_Pin */
  GPIO_InitStruct.Pin = LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Out2L1_Pin Out2L2_Pin Out2L3_Pin Out3L1_Pin
                             Out3L2_Pin Out3L3_Pin Out1L1_Pin Out1L2_Pin
                             Out1L3_Pin */
    GPIO_InitStruct.Pin = Out2L1_Pin|Out2L2_Pin|Out2L3_Pin|Out3L1_Pin
                            |Out3L2_Pin|Out3L3_Pin|Out1L1_Pin|Out1L2_Pin
                            |Out1L3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RS485_3_WE_Pin */
  GPIO_InitStruct.Pin = RS485_3_WE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_3_WE_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
uint64_t get_Value(ParamID_t messValue){
	uint64_t Spannung = 0;
	sml_file* tempReq = NULL, *tempResp = NULL;//sml request & response files
	uint8_t tempBufRx[2048] = { 0 };//Rx buffer
	int32_t tempR = 0;//mem return values
	OBISDataChain_t* tempData = NULL;//data chain pointer

//		PID_APlus,
//		PID_Voltage_L1,
//		PID_Voltage_L2,
//		PID_Voltage_L3,
	if ((tempR = SMLF_CreateGetPPReqFilePID(messValue, &tempReq))) TRACE(LMN_GetErrorMessage(tempR)); //generate sml file
	uint8_t* tempBufTx = SMLF_FileGetBuf(tempReq);
	uint16_t tempBufTxLen = SMLF_FileGetBufLen(tempReq);

	//test message
//		uint8_t tempXXX[] = { 0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x01, 0x02, 0x65, 0x13, 0x47, 0x16, 0x00, 0x00 };

	//tx & rx
	HAL_GPIO_WritePin(RS485_3_WE_PORT, RS485_3_WE_PIN, GPIO_PIN_SET);
	HAL_StatusTypeDef tempS = HAL_UART_Transmit(&huart3, tempBufTx, tempBufTxLen, 2000 / portTICK_PERIOD_MS);//4000ms Tx timeout
	HAL_GPIO_WritePin(RS485_3_WE_PORT, RS485_3_WE_PIN, GPIO_PIN_RESET);

	HAL_Delay(5);//wait for Tx finished :(
	__HAL_UART_CLEAR_FLAG(&huart3, (UART_CLEAR_IDLEF | UART_CLEAR_TCF | UART_CLEAR_LBDF | UART_CLEAR_PEF | UART_CLEAR_FEF | UART_CLEAR_NEF | UART_CLEAR_OREF));//clear corrupted status from bus echo
	__HAL_UART_FLUSH_DRREGISTER(&huart3);//clear corrupted status from bus echo
	tempS = HAL_UART_Receive(&huart3, tempBufRx, sizeof(tempBufRx), 2000 / portTICK_PERIOD_MS);//4000ms Rx timeout

	//vTaskStartTrace();

	tempResp = SMLF_FileParseFromBuf(tempBufRx, sizeof(tempBufRx));//parse sml response file from Rx buffer
	if ((tempR = SMLF_ParseGetPPRespFile(tempResp, &tempData))) TRACE(LMN_GetErrorMessage(tempR));//parse data from sml response file

	//use tempData here!!!
	Spannung = tempData->Data.CReg.Value.UInt64; //Spannung L1

	OBIS_DataFreeChain(tempData);//free data chain
	SMLF_FileFree(tempReq);//free sml files...
	SMLF_FileFree(tempResp);

	return Spannung;
}


// Funktion, die die kleinste Spannung aus dem Array der Spannung (der drei Phasen) sucht

uint64_t Kleinste(uint64_t *Spannung){
	if(Spannung[0]==Spannung[1] && Spannung[0]==Spannung[2])
		return 0;
	uint64_t kleinste = Spannung[0];
	uint64_t phase = 1;
	int i = 0;
	for(i=1; i<3; i++){
		if(kleinste>Spannung[i]){
			kleinste = Spannung[i];
			phase = i + 1;
		}
	}
	return phase;
}

// Funktion, die die größte Spannung aus dem Array der Spannung (der drei Phasen) sucht

uint64_t Grosste(uint64_t Spannung[]){
	if(Spannung[0]==Spannung[1] && Spannung[0]==Spannung[2])
		return 0;
	uint64_t grosste = Spannung[0];
	uint64_t phase = 1;
	for(int i=1; i<3; i++){
		if(Spannung[i]>grosste){
			grosste = Spannung[i];
			phase = i+1;
		}
	}
	return phase;
}


// Funktion für das Umschalten des Output 1

void Umschaltung_Output1(uint64_t grosste_Spannung){
	uint16_t Out1_pin;
	uint16_t Out1_neu_pin;
	HAL_GPIO_WritePin(GPIOB, Out1L1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, Out1L2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, Out1L3_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	Output1 = grosste_Spannung;
	switch(grosste_Spannung){
	case 1:
		Out1_neu_pin = Out1L1_Pin;
		break;
	case 2:
		Out1_neu_pin = Out1L2_Pin;
		break;
	case 3:
		Out1_neu_pin = Out1L3_Pin;
		break;
	}

	HAL_GPIO_WritePin(GPIOB, Out1_neu_pin, GPIO_PIN_SET);
}

// Funktion für das Umschalten des Output 2

void Umschaltung_Output2(uint64_t grosste_Spannung){
	uint16_t Out2_pin;
	uint16_t Out2_neu_pin;
	HAL_GPIO_WritePin(GPIOB, Out2L1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, Out2L2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, Out2L3_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	Output2 = grosste_Spannung;
	switch(grosste_Spannung){
	case 1:
		Out2_neu_pin = Out2L1_Pin;
		break;
	case 2:
		Out2_neu_pin = Out2L2_Pin;
		break;
	case 3:
		Out2_neu_pin = Out2L3_Pin;
		break;
	}

	HAL_GPIO_WritePin(GPIOB, Out2_neu_pin, GPIO_PIN_SET);
}

// Funktion für das Umschalten des Output 3

void Umschaltung_Output3(uint64_t grosste_Spannung){
	uint16_t Out3_pin;
	uint16_t Out3_neu_pin;
	HAL_GPIO_WritePin(GPIOB, Out3L1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, Out3L2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, Out3L3_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	Output3 = grosste_Spannung;
	switch(grosste_Spannung){
	case 1:
		Out3_neu_pin = Out3L1_Pin;
		break;
	case 2:
		Out3_neu_pin = Out3L2_Pin;
		break;
	case 3:
		Out3_neu_pin = Out3L3_Pin;
		break;
	}

	HAL_GPIO_WritePin(GPIOB, Out3_neu_pin, GPIO_PIN_SET);
}

// Funktion für Umschaltregeln (gilt für Output 1,2 und 3)
// 1. Regel: Alle Spannungen der Phasen sind unterschiedlich
//          -> Umschalten der Phase mit dem größten Strom auf die Phase mit der größten Spannung
// 2. Regel: Alle Spannungen der Phasen sind gleich
//          -> Verteilen der Phasen auf Out 1 Phase 1, Out 2 Phase 2 und Out 3 Phase 3
// 3. Regel: Zwei Spannungen der Phasen sind gleich und kleiner als die dritte Spannung
//          -> Umschalten der Spannung (einer der kleineren und gleichen Spannungen) mit dem größten Strom auf die größte Spannung
// 4. Regel: Zwei Spannungen der Phasen sind gleich und größer als die dritte Spannung
//          -> Umschalten der kleineren Spannung auf eine der größeren Spannungen mit dem kleinsten Strom

// Umschaltregeln Output 1

void check_Output1(uint64_t Spannung[], uint64_t Strom[], uint64_t grossteSpannung, uint64_t grossteStrom){
	 if(Spannung[0]!=Spannung[1] && Spannung[1]!=Spannung[2] && Spannung[1]!=Spannung[2]){  // 1. Regel
		  if(Output1 == grossteStrom){
			  Umschaltung_Output1(grossteSpannung);
		  }
	  } else if(Spannung[0]==Spannung[1] && Spannung[0]==Spannung[2]){ // 2. Regel
		  Umschaltung_Output1(1);
	  } else {
		  if(Spannung[0]<Spannung[2] && Spannung[1]<Spannung[2]){ // 3. Regel
			  if(grossteStrom==Strom[0] || grossteStrom==Strom[1] || Strom[0]==Strom[1])
				  Umschaltung_Output1(3);
		  }else if(Spannung[0]<Spannung[1] && Spannung[2]<Spannung[1]){
			  if(grossteStrom==Strom[0] || grossteStrom==Strom[2] || Strom[0]==Strom[2])
				  Umschaltung_Output1(2);
		  } else if(Spannung[1]<Spannung[0] && Spannung[2]<Spannung[0]){
			  if(grossteStrom==Strom[1] || grossteStrom==Strom[2] || Strom[2]==Strom[1])
				  Umschaltung_Output1(1);

		  } else if (Spannung[0]>Spannung[2] && Spannung[1]>Spannung[2] ){ // 4. Regel
				if(Strom[0]<Strom[1]){
					Umschaltung_Output1(1);
				} else Umschaltung_Output1(2);
			}  else if (Spannung[0]>Spannung[1] && Spannung[2]>Spannung[1]){
				if(Strom[0]<Strom[2]){
					Umschaltung_Output1(1);
				} else Umschaltung_Output1(3);
			}  else if (Spannung[1]>Spannung[0] && Spannung[2]>Spannung[0]){
				if(Strom[2]<Strom[1]){
					Umschaltung_Output1(2);
				} else Umschaltung_Output1(3);
			}
	  }
}

// Umschaltregeln Output 2

void check_Output2(uint64_t Spannung[],uint64_t Strom[], uint64_t grossteSpannung, uint64_t grossteStrom){
	if(Spannung[0]!=Spannung[1] && Spannung[1]!=Spannung[2] && Spannung[1]!=Spannung[2]){
		  if(Output2 == grossteStrom){
			  Umschaltung_Output2(grossteSpannung);
		  }
	} else if(Spannung[0]==Spannung[1] && Spannung[0]==Spannung[2]){
		Umschaltung_Output2(2);
	} else {
		if(Spannung[0]<Spannung[2] && Spannung[1]<Spannung[2]){
		  if(grossteStrom==Strom[0] || grossteStrom==Strom[1] || Strom[0]==Strom[1])
			  Umschaltung_Output2(3);
	  }else if(Spannung[0]<Spannung[1] && Spannung[2]<Spannung[1]){
		  if(grossteStrom==Strom[0] || grossteStrom==Strom[2] || Strom[0]==Strom[2])
			  Umschaltung_Output2(2);
	  } else if(Spannung[1]<Spannung[0] && Spannung[2]<Spannung[0]){
		  if(grossteStrom==Strom[1] || grossteStrom==Strom[2] || Strom[2]==Strom[1])
			  Umschaltung_Output2(1);


		} else if (Spannung[0]>Spannung[2] && Spannung[1]>Spannung[2] ){
			if(Strom[0]<Strom[1]){
				Umschaltung_Output2(1);
			} else Umschaltung_Output2(2);
		}  else if (Spannung[0]>Spannung[1] && Spannung[2]>Spannung[1]){
			if(Strom[0]<Strom[2]){
				Umschaltung_Output2(1);
			} else Umschaltung_Output2(3);
		}  else if (Spannung[1]>Spannung[0] && Spannung[2]>Spannung[0]){
			if(Strom[2]<Strom[1]){
				Umschaltung_Output2(2);
			} else Umschaltung_Output2(3);
		}
	}
}

// Umschaltregeln Output 3

void check_Output3(uint64_t Spannung[], uint64_t Strom[], uint64_t grossteSpannung, uint64_t grossteStrom){
	if(Spannung[0]!=Spannung[1] && Spannung[1]!=Spannung[2] && Spannung[1]!=Spannung[2]){
	  if(Output3 == grossteStrom){
		  Umschaltung_Output3(grossteSpannung);
	  }
	} else if(Spannung[0]==Spannung[1] && Spannung[0]==Spannung[2]){
	  Umschaltung_Output3(3);
	} else {
		if(Spannung[0]<Spannung[2] && Spannung[1]<Spannung[2]){
		  if(grossteStrom==Strom[0] || grossteStrom==Strom[1] || Strom[0]==Strom[1])
			  Umschaltung_Output3(3);
	  }else if(Spannung[0]<Spannung[1] && Spannung[2]<Spannung[1]){
		  if(grossteStrom==Strom[0] || grossteStrom==Strom[2] || Strom[0]==Strom[2])
			  Umschaltung_Output3(2);
	  } else if(Spannung[1]<Spannung[0] && Spannung[2]<Spannung[0]){
		  if(grossteStrom==Strom[1] || grossteStrom==Strom[2] || Strom[2]==Strom[1])
			  Umschaltung_Output3(1);


		} else if (Spannung[0]>Spannung[2] && Spannung[1]>Spannung[2] ){
			if(Strom[0]<Strom[1]){
				Umschaltung_Output3(1);
			} else Umschaltung_Output3(2);
		}  else if (Spannung[0]>Spannung[1] && Spannung[2]>Spannung[1]){
			if(Strom[0]<Strom[2]){
				Umschaltung_Output3(1);
			} else Umschaltung_Output3(3);
		}  else if (Spannung[1]>Spannung[0] && Spannung[2]>Spannung[0]){
			if(Strom[2]<Strom[1]){
				Umschaltung_Output3(2);
			} else Umschaltung_Output3(3);
		}

	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  while(1)
  {
	  //Spannung und Strom Address
	  ParamID_t Spannungaddr[3] = {PID_Voltage_L1, PID_Voltage_L2, PID_Voltage_L3};
	  ParamID_t Stromaddr[3] = {PID_Current_L1, PID_Current_L2, PID_Current_L3};
	  uint64_t Spannung[3] = {0,0,0};
	  uint64_t Strom[3] = {0,0,0};
	  uint8_t Uart2Buf[200];

//	  OUTPUT1 (Messen und Überprüfen)

	  for(int i=0; i<3; i++){
		  Spannung[i] = get_Value(Spannungaddr[i]);
//    Umwandlung in Spannungswert OHNE Kommazahl -> 230.2 werden zu 230 V, Genauigkeit von +- 1V
		  Spannung[i] /= 10;
		  Strom[i] = get_Value(Stromaddr[i]);
		  sprintf((char*)Uart2Buf,"SpannungL%d: %u \t ", (i+1), Spannung[i]);
		  HAL_UART_Transmit(&huart2, Uart2Buf, 15, 400);

		  sprintf((char*)Uart2Buf,"StromL%d: %u\t ", (i+1), Strom[i]);
		  HAL_UART_Transmit(&huart2, Uart2Buf, 15, 400);
	  }
	  sprintf((char*)Uart2Buf,"Output%d: %u\t ", 1, Output1);
	  HAL_UART_Transmit(&huart2, Uart2Buf, 30, 400);
	  sprintf((char*)Uart2Buf,"Output%d: %u\t ", 2, Output2);
	  HAL_UART_Transmit(&huart2, Uart2Buf, 30, 400);
	  sprintf((char*)Uart2Buf,"Output%d: %u\t ", 3, Output3);
	  HAL_UART_Transmit(&huart2, Uart2Buf, 30, 400);

// Definition größte und kleinste Spannung, Strom

	  uint64_t grossteSpannung = Grosste(Spannung);
	  uint64_t grossteStrom = Grosste(Strom);
	  uint64_t kleinsteSpannung = Kleinste(Spannung);
	  uint64_t kleinsteStrom = Kleinste(Strom);

	  check_Output1(Spannung, Strom, grossteSpannung, grossteStrom);

//	  OUTPUT2 (Messen und Überprüfen)

	  for(int i=0; i<3; i++){
		  Spannung[i] = get_Value(Spannungaddr[i]);
		  Spannung[i] /= 10;
		  Strom[i] = get_Value(Stromaddr[i]);
	  }
	  grossteSpannung = Grosste(Spannung);
	  grossteStrom = Grosste(Strom);

	  check_Output2(Spannung, Strom, grossteSpannung, grossteStrom);

//	  OUTPUT3 (Messen und Überprüfen)

	  for(int i=0; i<3; i++){
		  Spannung[i] = get_Value(Spannungaddr[i]);
		  Spannung[i] /= 10;
		  Strom[i] = get_Value(Stromaddr[i]);
	  }
	  grossteSpannung = Grosste(Spannung);
	  grossteStrom = Grosste(Strom);

	  check_Output3(Spannung, Strom, grossteSpannung, grossteStrom);

// Senden der Informationen von größter Spannung und Strom an den PC

	  sprintf((char*)Uart2Buf, "grosste Spannung: %u \n" , grossteSpannung);
	  HAL_UART_Transmit(&huart2, Uart2Buf, 21, 100);
	  sprintf((char*)Uart2Buf, "grosste Strom: %u \n" , grossteStrom);
	  HAL_UART_Transmit(&huart2, Uart2Buf, 21, 100);



	  osDelay(5000);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StarttoggleLEDTask */
/**
* @brief Function implementing the toggleLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StarttoggleLEDTask */
void StarttoggleLEDTask(void *argument)
{
  /* USER CODE BEGIN StarttoggleLEDTask */
  /* Infinite loop */
  while(1)
  {
	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	  osDelay(800);
  }
  /* USER CODE END StarttoggleLEDTask */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
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
}
  /* USER CODE END Error_Handler_Debug */


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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
