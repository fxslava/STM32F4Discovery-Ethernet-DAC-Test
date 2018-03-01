#include "GPIOModule.h"

#include "stm32F4xx_hal.h"
#include "stm32F4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"

void InitGPIO()
{
	GPIO_InitTypeDef InPort = {0};
	GPIO_InitTypeDef OutPort = {0};
	
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();
	
	// Output pins
	OutPort.Pin	 =  GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_3  | GPIO_PIN_4 
								| GPIO_PIN_5  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9 
								| GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	OutPort.Mode	 = GPIO_MODE_OUTPUT_PP;
	OutPort.Pull	 = GPIO_NOPULL;
	OutPort.Speed = GPIO_SPEED_HIGH;
	
	// Initialize port
	HAL_GPIO_Init(GPIOD, &OutPort);
	
	// Output pins
	InPort.Pin	 =  GPIO_PIN_3  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9 
								| GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 
								| GPIO_PIN_14 | GPIO_PIN_15;
	InPort.Mode	 = GPIO_MODE_INPUT;
	InPort.Pull	 = GPIO_NOPULL;
	InPort.Speed = GPIO_SPEED_HIGH;
	
	// Initialize port
	HAL_GPIO_Init(GPIOE, &InPort);
}