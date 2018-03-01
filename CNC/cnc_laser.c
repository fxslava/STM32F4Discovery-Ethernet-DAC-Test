#include "cnc_laser.h"

DAC_HandleTypeDef hDAC;

float min(float x, float y)
{
	if (x < y) return x;
	else return y;
}

uint32_t SetDACValue(float v, CNC_MACHINE * machine)
{
	return ((uint32_t)(SetLaserPower(v, machine) * 4095)) & 0xfff;
}	

float SetLaserPower(float v, CNC_MACHINE * machine)
{
	float P = /*config.minPower + */(machine->config.maxPower/* - config.minPower*/) * v / machine->init.velocity_max;
	if (P < machine->config.minPower) P = machine->config.minPower;
	return P;
}

void CNC_LaserPort_Init(void)
{	
	DAC_ChannelConfTypeDef configCH1 = {0};
	GPIO_InitTypeDef portLaser = {0};
	__GPIOB_CLK_ENABLE();
	
	// Laser port init
	portLaser.Pin				= GPIO_PIN_15; // Laser ON/OFF
	portLaser.Mode			= GPIO_MODE_OUTPUT_PP;
	portLaser.Pull			= GPIO_NOPULL;
	portLaser.Speed			= GPIO_SPEED_FAST;
	portLaser.Alternate	= GPIO_AF0_RTC_50Hz;

	// Initialize port
	HAL_GPIO_Init(GPIOB, &portLaser);
	
	// Set default state
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
	
	__GPIOA_CLK_ENABLE();
	
	// Laser analog pin
	portLaser.Pin				= GPIO_PIN_4; // Laser ON/OFF
	portLaser.Mode			= GPIO_MODE_ANALOG;
	portLaser.Pull			= GPIO_NOPULL;
	//portLaser.Speed			= GPIO_SPEED_FAST;
	//portLaser.Alternate	= GPIO_AF0_RTC_50Hz;
	
	// Initialize port
	HAL_GPIO_Init(GPIOA, &portLaser);
	
	// Init DAC
	hDAC.Instance = DAC;
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	HAL_DAC_Init(&hDAC);
	
	// Config channel 1
	configCH1.DAC_Trigger = DAC_TRIGGER_NONE;
	configCH1.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	
	HAL_DAC_ConfigChannel(&hDAC, &configCH1, DAC_CHANNEL_1);
	
	// Always on DAC
	HAL_DAC_Start(&hDAC, DAC_CHANNEL_1);
}
