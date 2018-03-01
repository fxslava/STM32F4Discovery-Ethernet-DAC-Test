#ifndef __CNC_LASER_HH
#define __CNC_LASER_HH

#include "cnc_def.h"
#include "cnc_machine.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dac.h"

extern DAC_HandleTypeDef hDAC;

#define DACC1(x, machine) HAL_DAC_SetValue(&hDAC, DAC_CHANNEL_1, DAC_ALIGN_12B_R, SetDACValue(x, machine))
#define DACC2(x, machine) HAL_DAC_SetValue(&hDAC, DAC_CHANNEL_2, DAC_ALIGN_12B_R, SetDACValue(x, machine))

uint32_t SetDACValue(float v, CNC_MACHINE * machine);
void CNC_LaserPort_Init(void);
float SetLaserPower(float v, CNC_MACHINE * machine);
#endif
