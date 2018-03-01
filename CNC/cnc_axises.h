#include <stdint.h>
#include "cnc_def.h"

#define CNC_USE_DMA
#define CNC_PRECALC

extern CNC_AXIS CNC_XAxis;
extern CNC_AXIS CNC_YAxis;
extern CNC_AXIS CNC_ZAxis;
extern CNC_AXIS CNC_AAxis;

extern TIM_HandleTypeDef hTIM2;
extern TIM_HandleTypeDef hTIM3;
extern TIM_HandleTypeDef hTIM4;
extern TIM_HandleTypeDef hTIM5;

extern DMA_HandleTypeDef hDMA_X;
extern DMA_HandleTypeDef hDMA_Y;
extern DMA_HandleTypeDef hDMA_Z;
extern DMA_HandleTypeDef hDMA_A;

extern void CalculatePeriod16(uint32_t interval, uint16_t* period, uint16_t* prescale);

extern HAL_StatusTypeDef CNC_Axis_Init(CNC_AxisTypeDef axisType);
extern HAL_StatusTypeDef CNC_Axis_Start(CNC_AxisTypeDef axisType, uint32_t time, int32_t position);

extern HAL_StatusTypeDef CNC_Axis_Prepare(CNC_AxisTypeDef axisType, uint32_t time, int32_t position);
extern HAL_StatusTypeDef CNC_Axis_StartX(void);
extern HAL_StatusTypeDef CNC_Axis_StartY(void);
extern HAL_StatusTypeDef CNC_Axis_StartZ(void);
extern HAL_StatusTypeDef CNC_Axis_StartA(void);

extern HAL_StatusTypeDef CNC_Axis_Init_Masked(CNC_AxisTypeDef axisType, uint8_t mask);
extern HAL_StatusTypeDef CNC_Axis_Prepare_Masked(CNC_AxisTypeDef axisType, uint8_t mask, uint32_t time, int32_t position);
extern HAL_StatusTypeDef CNC_Axis_Start_Masked(CNC_AxisTypeDef axisType, uint8_t mask, uint32_t time, int32_t position);

extern uint32_t CalculateMask(uint32_t steps, uint32_t ord);
