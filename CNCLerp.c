#include "CNCLerp.h"
#include "stm32f4xx_hal_gpio.h"

uint16_t brev16(uint16_t value)
{
	const uint16_t mask0 = 0x5555;
	const uint16_t mask1 = 0x3333;
	const uint16_t mask2 = 0x0F0F;
	const uint16_t mask3 = 0x00FF;

	value = (((~mask0) & value) >> 1) | ((mask0 & value) << 1);
	value = (((~mask1) & value) >> 2) | ((mask1 & value) << 2);
	value = (((~mask2) & value) >> 4) | ((mask2 & value) << 4);
	value = (((~mask3) & value) >> 8) | ((mask3 & value) << 8);

	return value;
}

uint8_t brev8(uint8_t value)
{
	const uint8_t mask0 = 0x55;
	const uint8_t mask1 = 0x33;
	const uint8_t mask2 = 0x0F;

	value = (((~mask0) & value) >> 1) | ((mask0 & value) << 1);
	value = (((~mask1) & value) >> 2) | ((mask1 & value) << 2);
	value = (((~mask2) & value) >> 4) | ((mask2 & value) << 4);

	return value;
}

uint8_t brev4(uint8_t value)
{
	const uint8_t mask0 = 0x5;
	const uint8_t mask1 = 0x3;

	value = (((~mask0) & value) >> 1) | ((mask0 & value) << 1);
	value = (((~mask1) & value) >> 2) | ((mask1 & value) << 2);

	return value;
}

uint8_t brev2(uint8_t value)
{
	const uint8_t mask0 = 0x1;

	value = (((~mask0) & value) >> 1) | ((mask0 & value) << 1);

	return value;
}

uint32_t brev(uint32_t value, uint8_t digits)
{
	uint32_t result = 0;
	uint32_t x = value;
	uint8_t d = digits;

	if ((digits & 0x10) != 0)	{
		d -= 16;	result |= (brev16(x & 0xffff) << d);	x >>= 16;	}

	if ((digits & 0x8) != 0)	{
		d -= 8;		result |= (brev8(x & 0xff		) << d);	x >>= 8;	}

	if ((digits & 0x4) != 0)	{
		d -= 4;		result |= (brev4(x & 0xf		) << d);	x >>= 4;	}

	if ((digits & 0x2) != 0)	{
		d -= 2;		result |= (brev2(x & 0x3		) << d);	x >>= 2;	}

	if ((digits & 0x1) != 0)	{
		d -= 1;		result |= x & 0x1;										x >>= 1;	}

	return result;
}

void UpdateCNCLerp(eCNCLerp_t* lerp, uint16_t period, bool *update)
{
	uint16_t i = lerp->counter;
	uint16_t hashCode = ~i & (i + 1);
	lerp->counter = i + 1;
	
	// Update Axises step
	if ((hashCode & lerp->AxisMask[0] & 0x7FFF) != 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	
	if ((hashCode & lerp->AxisMask[1] & 0x7FFF) != 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
	
	if ((hashCode & lerp->AxisMask[2] & 0x7FFF) != 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
	
	// Update Axises dir
	if ((lerp->AxisMask[0] & 0x8000) != 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	
	if ((lerp->AxisMask[1] & 0x8000) != 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	
	if ((lerp->AxisMask[2] & 0x8000) != 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	
	*update = false;
	if (i == lerp->period)
	{
		lerp->counter = 0;
		*update = true;
	}
}
