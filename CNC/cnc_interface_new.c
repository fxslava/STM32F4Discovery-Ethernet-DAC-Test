#include <cmsis_os.h>                                           // CMSIS RTOS header file
// CNC
#include "cnc_interface.h"

//CMSIS
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_dma.h"
//#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"

USART_HandleTypeDef hUSART6;
UART_HandleTypeDef hUART6;

void CNC_Receive(void* data, uint32_t num)
{
	//HAL_USART_Receive(&hUSART6, (uint8_t*)data, num, 1024);
	//while (HAL_UART_Receive(&hUART6, (uint8_t*)data, num, 0) == HAL_TIMEOUT) osThreadYield();
	HAL_UART_Receive(&hUART6, (uint8_t*)data, num, 30);
}

void CNC_Transmit(void* data, uint32_t num)
{
	//HAL_USART_Receive(&hUSART6, (uint8_t*)data, num, 1024);
	//while (HAL_UART_Transmit(&hUART6, (uint8_t*)data, num, 0) == HAL_TIMEOUT) osThreadYield();
	HAL_UART_Transmit(&hUART6, (uint8_t*)data, num, 30);
}

ARM_DRIVER_USART* USART_Init(osThreadId tid_Thread)
{
	GPIO_InitTypeDef	initGPIO = {0};
	
	// Initialize GPIO for USART TX
	__HAL_RCC_GPIOC_CLK_ENABLE();
	initGPIO.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	initGPIO.Mode = GPIO_MODE_AF_PP;
	initGPIO.Pull = GPIO_PULLUP;
	initGPIO.Speed = GPIO_SPEED_HIGH;
	initGPIO.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC, &initGPIO);
	
	// Initialize UART6
	__HAL_RCC_USART6_CLK_ENABLE();
	hUART6.Instance = USART6;
  hUART6.Init.BaudRate = 9600;
  hUART6.Init.WordLength = UART_WORDLENGTH_8B;
  hUART6.Init.StopBits = UART_STOPBITS_1;
  hUART6.Init.Parity = UART_PARITY_NONE;
  hUART6.Init.Mode = UART_MODE_TX_RX;
  hUART6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hUART6.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&hUART6);
	
	return NULL;
}
