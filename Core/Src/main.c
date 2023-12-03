/*******************************************************************************************************************************//**
 *
 * @file		main.c
 * @proyect		EXAMPLE
 * @author		Velazquez Lautaro¿
 *
 **********************************************************************************************************************************/


/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/

#include "main.h"
#include "cmsis_os.h"
#include "Board/lis3mdl.h"
#include "Board/filesystem.h"
#include "string.h"
#include <stdbool.h>

/***********************************************************************************************************************************
 *** PRIVATE DEFINES
 **********************************************************************************************************************************/



/***********************************************************************************************************************************
 *** PRIVATE MACROS
 **********************************************************************************************************************************/



/***********************************************************************************************************************************
 *** PRIVATE DATA
 **********************************************************************************************************************************/



/***********************************************************************************************************************************
 *** PUBLIC VARIABLES
 **********************************************************************************************************************************/

I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

/***********************************************************************************************************************************
 *** PRIVATE VARIABLES
 **********************************************************************************************************************************/

osThreadId defaultTaskHandle;
osMessageQId myQueue01Handle;
osTimerId myTimer01Handle;
osSemaphoreId myBinarySem01Handle;

static uint64_t ID = 0;
static uint64_t pData;

/***********************************************************************************************************************************
 *** PRIVATE FUNCTION PROTOTYPE
 **********************************************************************************************************************************/

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);
void Callback01(void const * argument);

/***********************************************************************************************************************************
 *** PUBLIC FUNCTIONS
 **********************************************************************************************************************************/

int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	lis3mdl_init();
	filesystem_init();

	HAL_UART_Receive_IT(&huart1, (uint8_t *)&pData, sizeof(pData));

	/* Create the semaphores(s) */
	/* definition and creation of myBinarySem01 */
	osSemaphoreDef(myBinarySem01);
	myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

	/* Create the timer(s) */
	/* definition and creation of myTimer01 */
	osTimerDef(myTimer01, Callback01);
	myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);
	osTimerStart(myTimer01Handle, 1000);

	/* Create the queue(s) */
	/* definition and creation of myQueue01 */
	osMessageQDef(myQueue01, 16, uint64_t);
	myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

	/* definition and creation of defaultTask */
	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osKernelStart();

	while (1)
	{
	}
}

/***********************************************************************************************************************************
 *** PRIVATE FUNCTION
 **********************************************************************************************************************************/

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
void StartDefaultTask(void const * argument)
{
	const char* pError = "ERROR ID NO ENCONTRADO\0";
	Measurements data;
	for(;;)
	{
		osEvent retvalue = osMessageGet(myQueue01Handle, 4000);

		if (retvalue.status == osEventMessage)
		{
			if (osSemaphoreWait(myBinarySem01Handle, osWaitForever) == osOK)
			{
				bool temp = filesystem_read(*((uint64_t *)retvalue.value.p), &data);
				osSemaphoreRelease(myBinarySem01Handle);
				if (temp)
					HAL_UART_Transmit_IT(&huart1, (uint8_t *)&data, sizeof(Measurements));
				else
					HAL_UART_Transmit_IT(&huart1, (uint8_t *)pError, strlen(pError));
			}
		}
	}
}

/**
  * @brief  button function callback.
  * @param  argument: GPIO_Pin represent the boton
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	Measurements data;
	if (GPIO_Pin == GPIO_PIN_3)
	{
		if (osSemaphoreWait(myBinarySem01Handle, osWaitForever) == osOK)
		{
			filesystem_read(ID, &data);
			osSemaphoreRelease(myBinarySem01Handle);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&data, sizeof(Measurements));
		}
	}
}

/**
  * @brief  runs every one second.
  * @param  argument: unused..
  * @retval None
  */
void Callback01(void const * argument)
{
	lis3mdl_data temp = lis3mdl_read();
	Measurements data;
	data.id = ID++;
	memcpy(&data.measurements, &temp, 8);
	if (osSemaphoreWait(myBinarySem01Handle, osWaitForever) == osOK)
	{
		filesystem_write(data);
		osSemaphoreRelease(myBinarySem01Handle);
	}
}

/**
  * @brief  is called when data is received by the uart.
  * @param  huart: represent the uart.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		osMessagePut(myQueue01Handle, pData, 1000);
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&pData, sizeof(pData));
	}
}

/**********************************************************************************************************************************/

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 400000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
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
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SC_GPIO_Port, SC_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : PA3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : SC_Pin */
	GPIO_InitStruct.Pin = SC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SC_GPIO_Port, &GPIO_InitStruct);
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

/*--------------------------------------------------------------------------------------------------------------------------------*/
