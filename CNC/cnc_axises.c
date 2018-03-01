#include <cmsis_os.h>   
#include "cnc_math.h"
#include "cnc_axises.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_dma.h"

extern osThreadId tid_Thread_CNC; 

TIM_HandleTypeDef hTIM2;
TIM_HandleTypeDef hTIM3;
TIM_HandleTypeDef hTIM4;
TIM_HandleTypeDef hTIM5;

DMA_HandleTypeDef hDMA_X;
DMA_HandleTypeDef hDMA_Y;
DMA_HandleTypeDef hDMA_Z;
DMA_HandleTypeDef hDMA_A;

CNC_AXIS CNC_XAxis;
CNC_AXIS CNC_YAxis;
CNC_AXIS CNC_ZAxis;
CNC_AXIS CNC_AAxis;

void CNC_XAxis_init()
{	
	GPIO_InitTypeDef portX = {0};
	DMA_InitTypeDef initDMA;
	__GPIOB_CLK_ENABLE();
	
	// TIM OC pin
	portX.Pin				= GPIO_PIN_11; // XSTEP
	portX.Mode			= GPIO_MODE_AF_PP;//GPIO_MODE_OUTPUT_PP;
	portX.Pull			= GPIO_NOPULL;
	portX.Speed			= GPIO_SPEED_LOW;//GPIO_SPEED_FAST;
	portX.Alternate	= GPIO_AF1_TIM2;
	// Initialize pin
	HAL_GPIO_Init(GPIOB, &portX);
	
	// DIR pins
	portX.Pin				= GPIO_PIN_12; // XDIR
	portX.Mode			= GPIO_MODE_OUTPUT_PP;
	portX.Alternate = GPIO_AF0_RTC_50Hz;
	// Initialize port
	HAL_GPIO_Init(GPIOB, &portX);
	
	// Set default state
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	
#ifdef CNC_USE_DMA
	// Init DMA stream
	__DMA1_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	initDMA.Channel = DMA_CHANNEL_3;
	initDMA.Direction = DMA_MEMORY_TO_PERIPH;
	initDMA.PeriphInc = DMA_PINC_DISABLE;
	initDMA.MemInc = DMA_MINC_ENABLE;
	initDMA.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // For TIM2
	initDMA.MemDataAlignment = DMA_MDATAALIGN_WORD; // For CNC_AXIS data
	initDMA.Mode = DMA_NORMAL;
	initDMA.Priority = DMA_PRIORITY_HIGH;
	initDMA.FIFOMode = DMA_FIFOMODE_DISABLE;
	initDMA.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	initDMA.MemBurst = DMA_MBURST_SINGLE;
	initDMA.PeriphBurst = DMA_PBURST_SINGLE;
	
	hDMA_X.Instance = DMA1_Stream1;
	hDMA_X.Init = initDMA;
	
	HAL_DMA_Init(&hDMA_X);

	hTIM2.hdma[TIM_DMA_ID_UPDATE] = &hDMA_X;
	
	hDMA_X.Parent = &hTIM2;
#endif
}

void CNC_YAxis_init()
{	
	GPIO_InitTypeDef portY = {0};
	DMA_InitTypeDef initDMA;
	__GPIOB_CLK_ENABLE();
	
	// TIM OC pin
	portY.Pin				= GPIO_PIN_1; // YSTEP
	portY.Mode			= GPIO_MODE_AF_PP;
	portY.Pull			= GPIO_NOPULL;
	portY.Speed			= GPIO_SPEED_LOW;//GPIO_SPEED_FAST;
	portY.Alternate	= GPIO_AF2_TIM3;
	// Initialize pin
	HAL_GPIO_Init(GPIOB, &portY);
	
	// DIR, EN pins
	portY.Pin				= GPIO_PIN_0; // YDIR, YEN
	portY.Mode			= GPIO_MODE_OUTPUT_PP;
	portY.Alternate = GPIO_AF0_RTC_50Hz;
	// Initialize port
	HAL_GPIO_Init(GPIOB, &portY);
	
	// Set default state
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	
#ifdef CNC_USE_DMA
	// Init DMA stream
	__DMA1_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
	initDMA.Channel = DMA_CHANNEL_5;
	initDMA.Direction = DMA_MEMORY_TO_PERIPH;
	initDMA.PeriphInc = DMA_PINC_DISABLE;
	initDMA.MemInc = DMA_MINC_ENABLE;
	initDMA.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // For TIM3
	initDMA.MemDataAlignment = DMA_MDATAALIGN_WORD; // For CNC_AXIS data
	initDMA.Mode = DMA_NORMAL;
	initDMA.Priority = DMA_PRIORITY_HIGH;
	initDMA.FIFOMode = DMA_FIFOMODE_DISABLE;
	initDMA.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	initDMA.MemBurst = DMA_MBURST_SINGLE;
	initDMA.PeriphBurst = DMA_PBURST_SINGLE;
	
	hDMA_Y.Instance = DMA1_Stream2;
	hDMA_Y.Init = initDMA;
	
	HAL_DMA_Init(&hDMA_Y);

	hTIM3.hdma[TIM_DMA_ID_UPDATE] = &hDMA_Y;
	hDMA_Y.Parent = &hTIM3;
#endif
}

HAL_StatusTypeDef CNC_Axis_Init(CNC_AxisTypeDef axisType)
{
	TIM_OC_InitTypeDef initOC;
	HAL_StatusTypeDef status = HAL_OK;
	TIM_HandleTypeDef* hTIM;
	CNC_AXIS* pAxis;
	
	initOC.OCMode = TIM_OCMODE_PWM1;
	initOC.OCPolarity = TIM_OCPOLARITY_LOW;
	initOC.Pulse = 0xffff;
	initOC.OCFastMode = TIM_OCFAST_ENABLE;
	
	// Set instance
	switch (axisType)
	{
		case CNC_X_AXIS:
				// Enable clock for TIM2
				__TIM2_CLK_ENABLE();
				// Enable interrupt request from TIM2
				HAL_NVIC_EnableIRQ(TIM2_IRQn);
				// Initialize GPIO pins
				CNC_XAxis_init();
		
				hTIM2.Instance = TIM2;
				hTIM = &hTIM2;
				pAxis = &CNC_XAxis;
			break;
		case CNC_Y_AXIS:
				// Enable clock for TIM3
				__TIM3_CLK_ENABLE();
				// Enable interrupt request from TIM2
				HAL_NVIC_EnableIRQ(TIM3_IRQn);
				// Initialize GPIO pins
				CNC_YAxis_init();
		
				hTIM3.Instance = TIM3;
				hTIM = &hTIM3;
				pAxis = &CNC_YAxis;
			break;
		case CNC_Z_AXIS:
				// Enable clock for TIM4
				__TIM4_CLK_ENABLE();
				// Enable interrupt request from TIM2
				HAL_NVIC_EnableIRQ(TIM4_IRQn);
		
				hTIM4.Instance = TIM4;
				hTIM = &hTIM4;
				pAxis = &CNC_ZAxis;
			break;
		case CNC_A_AXIS:
				// Enable clock for TIM5
				__TIM5_CLK_ENABLE();
				// Enable interrupt request from TIM2
				HAL_NVIC_EnableIRQ(TIM5_IRQn);
		
				hTIM5.Instance = TIM5;
				hTIM = &hTIM5;
				pAxis = &CNC_AAxis;
			break;
		default: return HAL_ERROR;
	}
	
	// Base init param
	hTIM->Init.Prescaler = 15;
	hTIM->Init.CounterMode = TIM_COUNTERMODE_UP;
	hTIM->Init.Period = 0xffff;
	hTIM->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	// init Axis
	pAxis->period = hTIM->Init.Period;
	pAxis->prescale = hTIM->Init.Prescaler;
	pAxis->mask = 0;
	pAxis->direction = AXIS_FORWARD;
	pAxis->src_position = 0;
	pAxis->dst_position = 0;
	pAxis->status = AXIS_READY;
	pAxis->mem = AXIS_MEM0;
	
	// Initialize TIM
	status = HAL_TIM_OC_Init(hTIM);
	if (status != HAL_OK) return status;
	
	// Initialize OC channel 4
	TIM_OC4_SetConfig(hTIM->Instance, &initOC);
	
	return status;
}

void CalculatePeriod16(uint32_t interval, uint16_t* period, uint16_t* prescale)
{
	uint32_t x = interval;
	*prescale = 1;
	while (x >= 65536)
	{
		x >>= 1;
		*prescale <<= 1;
	}
	
	*period = (uint16_t)x;
	(*prescale)--;
}

void TIM_DMAPeriodElapsedCplt(DMA_HandleTypeDef *hdma)
{
  TIM_HandleTypeDef* htim = ( TIM_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  
  htim->State= HAL_TIM_STATE_READY;
  
  HAL_TIM_PeriodElapsedCallback(htim);
}

HAL_StatusTypeDef CNC_Axis_Prepare(CNC_AxisTypeDef axisType, uint32_t time, int32_t position)
{
	//TIM_HandleTypeDef* hTIM = NULL;
	CNC_AXIS* pAxis;
	
	int32_t steps;
	uint16_t Time;
	uint16_t Prescale;
	uint16_t i;
	
	CalculatePeriod16(time, &Time, &Prescale);
	
	switch (axisType)
	{
		case CNC_X_AXIS:	/*hTIM = &hTIM2;*/ pAxis = &CNC_XAxis;
			break;
		case CNC_Y_AXIS:	/*hTIM = &hTIM3;*/ pAxis = &CNC_YAxis;
			break;
		case CNC_Z_AXIS:	/*hTIM = &hTIM4;*/ pAxis = &CNC_ZAxis;
			break;
		case CNC_A_AXIS:	/*hTIM = &hTIM5;*/ pAxis = &CNC_AAxis;
			break;
	}
	if (position == pAxis->src_position) 
	{
		//osSignalSet(tid_Thread_CNC, 0x1 << axisType);
		pAxis->status &= AXIS_STOP;
		pAxis->status |= AXIS_STOP;
		return HAL_OK;
	}
	pAxis->status |= AXIS_READY;
	
	if (position > pAxis->src_position)
	{
		if (axisType == CNC_Y_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		if (axisType == CNC_X_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		pAxis->direction = AXIS_FORWARD;
		steps = position - pAxis->src_position;
	}
	else
	{
		if (axisType == CNC_Y_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		if (axisType == CNC_X_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		pAxis->direction = AXIS_BACKWARD;
		steps = pAxis->src_position - position;
	}
	pAxis->steps = steps;
	
	if (steps != 0)
		pAxis->mask = CalculateMask(steps, Time % steps);
	else
		pAxis->mask = 0;
	
	pAxis->dst_position = position;
	if (steps != 0)
		pAxis->period = Time / steps;
	else
		pAxis->period = 1;
	
	pAxis->prescale = Prescale;
	
	// Calculate DMA buffer
	if (TIM_DMA_BUFFER_SIZE < steps) __breakpoint(0);
	
	if (pAxis->mem == AXIS_MEM0)
	{
		for (i = 0; i < steps; i++)
		{
			if ((~i & (i+1) & pAxis->mask) != 0)
				pAxis->dmaBuffer[i] = pAxis->period + 1;
			else
				pAxis->dmaBuffer[i] = pAxis->period;
		}
	} else
	{
		for (i = 0; i < steps; i++)
		{
			if ((~i & (i+1) & pAxis->mask) != 0)
				pAxis->dmaBuffer1[i] = pAxis->period + 1;
			else
				pAxis->dmaBuffer1[i] = pAxis->period;
		}
	}

#ifdef CNC_PRECALC
	pAxis->src_position = pAxis->dst_position;
#endif
	
	return HAL_OK;
}

HAL_StatusTypeDef HAL_DMA_Start_IT_Fast(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
	/* Process locked */
  __HAL_LOCK(hdma);

  /* Change DMA peripheral state */
  hdma->State = HAL_DMA_STATE_BUSY;

  /* Disable the peripheral */
  __HAL_DMA_DISABLE(hdma);

  /* Configure the source, destination address and the data length */
  /* Configure DMA Stream data length */
  hdma->Instance->NDTR = DataLength;

  /* Peripheral to Memory */
  if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    /* Configure DMA Stream destination address */
    hdma->Instance->PAR = DstAddress;

    /* Configure DMA Stream source address */
    hdma->Instance->M0AR = SrcAddress;
  }
  /* Memory to Peripheral */
  else
  {
    /* Configure DMA Stream source address */
    hdma->Instance->PAR = SrcAddress;
    
    /* Configure DMA Stream destination address */
    hdma->Instance->M0AR = DstAddress;
  }

  /* Enable the transfer complete interrupt */
  __HAL_DMA_ENABLE_IT(hdma, DMA_IT_TC);

   /* Enable the Peripheral */
  __HAL_DMA_ENABLE(hdma);
	
	return HAL_OK;
}

void CNC_Axis_Config(TIM_HandleTypeDef* hTIM, CNC_AXIS* pAxis)
{
	hTIM->State = HAL_TIM_STATE_BUSY;
	
	// Set period auto reload register 
	__HAL_TIM_SetAutoreload(hTIM, pAxis->period);
	
	// Set compare register 
	__HAL_TIM_SetCompare(hTIM, TIM_CHANNEL_4, pAxis->period >> 1);
	
	// Set prescale register 
	hTIM->Instance->PSC = pAxis->prescale;
	
	// Generate update event 
	hTIM->Instance->EGR = TIM_EGR_UG;
	
	// Enable output compare module 
	TIM_CCxChannelCmd(hTIM->Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
	
	// Set counter value to zero 
	__HAL_TIM_SetCounter(hTIM, 0);
	
	// Clear update interrupt flag 
	__HAL_TIM_CLEAR_FLAG(hTIM, TIM_FLAG_UPDATE);

	// Set the DMA Period elapsed callback 
  hTIM->hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = TIM_DMAPeriodElapsedCplt;
     
  // Set the DMA error callback 
  hTIM->hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = HAL_TIM_DMAError ;
	
	// Enable the DMA Stream 
	if (pAxis->mem == AXIS_MEM0)
	{
		HAL_DMA_Start_IT_Fast(hTIM->hdma[TIM_DMA_ID_UPDATE], (uint32_t)pAxis->dmaBuffer, (uint32_t)&hTIM->Instance->ARR, (int32_t)pAxis->steps);
		pAxis->mem = AXIS_MEM1;
	}
	else
	{
		HAL_DMA_Start_IT_Fast(hTIM->hdma[TIM_DMA_ID_UPDATE], (uint32_t)pAxis->dmaBuffer1, (uint32_t)&hTIM->Instance->ARR, (int32_t)pAxis->steps);
		pAxis->mem = AXIS_MEM0;
	}
	
	// Enable the TIM Update DMA request 
  __HAL_TIM_ENABLE_DMA(hTIM, TIM_DMA_UPDATE);
}

HAL_StatusTypeDef CNC_Axis_StartX(void)
{
	if ((CNC_XAxis.status & AXIS_READY) != 0)
	{
		CNC_Axis_Config(&hTIM2, &CNC_XAxis);
		__HAL_TIM_ENABLE(&hTIM2);
		CNC_XAxis.status = AXIS_BUSY;
	}
	else
		osSignalSet(tid_Thread_CNC, 0x1);
	
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_StartY(void)
{	
	if ((CNC_YAxis.status & AXIS_READY) != 0)
	{
		CNC_Axis_Config(&hTIM3, &CNC_YAxis);
		__HAL_TIM_ENABLE(&hTIM3);
		CNC_YAxis.status = AXIS_BUSY;
	}
	else
		osSignalSet(tid_Thread_CNC, 0x2);
	
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_StartZ(void)
{	
	if (CNC_ZAxis.status == AXIS_READY)
		__HAL_TIM_ENABLE(&hTIM4);
	else
		osSignalSet(tid_Thread_CNC, 0x4);
	
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_StartA(void)
{	
	if (CNC_AAxis.status == AXIS_READY)
		__HAL_TIM_ENABLE(&hTIM5);
	else
		osSignalSet(tid_Thread_CNC, 0x8);
	
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_Start(CNC_AxisTypeDef axisType, uint32_t time, int32_t position)
{
	TIM_HandleTypeDef* hTIM;
	CNC_AXIS* pAxis;
	uint16_t steps;
	uint16_t Time;
	uint16_t Prescale;
#ifndef CNC_USE_MULTI_BUFFER
	uint16_t i;
#endif
	
	CalculatePeriod16(time, &Time, &Prescale);
	
	switch (axisType)
	{
		case CNC_X_AXIS:	hTIM = &hTIM2; pAxis = &CNC_XAxis;
			break;
		case CNC_Y_AXIS:	hTIM = &hTIM3; pAxis = &CNC_YAxis;
			break;
		case CNC_Z_AXIS:	hTIM = &hTIM4; pAxis = &CNC_ZAxis;
			break;
		case CNC_A_AXIS:	hTIM = &hTIM5; pAxis = &CNC_AAxis;
			break;
	}
	
	if (position == pAxis->src_position) 
	{
		osSignalSet(tid_Thread_CNC, 0x1 << axisType);
		pAxis->status = AXIS_STOP;
		return HAL_OK;
	}
	
	if (position > pAxis->src_position)
	{
		if (axisType == CNC_Y_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		if (axisType == CNC_X_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		pAxis->direction = AXIS_FORWARD;
		steps = position - pAxis->src_position;
	}
	else
	{
		if (axisType == CNC_Y_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		if (axisType == CNC_X_AXIS)
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		pAxis->direction = AXIS_BACKWARD;
		steps = pAxis->src_position - position;
	}
	
	if (steps != 0)
		pAxis->mask = CalculateMask(steps, Time % steps);
	else
		pAxis->mask = 0;
	
	pAxis->dst_position = position;
	if (steps != 0)
		pAxis->period = Time / steps;
	else
		pAxis->period = 1;
	
	pAxis->status = AXIS_BUSY;
	pAxis->prescale = Prescale;
	
#ifdef CNC_USE_DMA
#ifndef CNC_USE_MULTI_BUFFER
	// Calculate DMA buffer
	if (TIM_DMA_BUFFER_SIZE < steps) __breakpoint(0);
	
	for (i = 0; i < steps; i++)
	{
		if ((~i & (i+1) & pAxis->mask) != 0)
			pAxis->dmaBuffer[i] = pAxis->period + 1;
		else
			pAxis->dmaBuffer[i] = pAxis->period;
	}
#endif
#endif
	
	/* Set period auto reload register */
	__HAL_TIM_SetAutoreload(hTIM, pAxis->period);
	
	/* Set compare register */
	__HAL_TIM_SetCompare(hTIM, TIM_CHANNEL_4, pAxis->period >> 1);
	
	/* Set prescale register */
	hTIM->Instance->PSC = pAxis->prescale;
	
	/* Enable output compare module */
	TIM_CCxChannelCmd(hTIM->Instance, TIM_CHANNEL_4, TIM_CCx_ENABLE);
	
	/* Generate update event */
	hTIM->Instance->EGR = TIM_EGR_UG;
	
	/* Set counter value to zero */
	__HAL_TIM_SetCounter(hTIM, 0);
	
	/* Clear update interrupt flag */
	__HAL_TIM_CLEAR_FLAG(hTIM, TIM_FLAG_UPDATE);
	
#ifndef CNC_USE_DMA
	// Enable interrupt & timer
	HAL_TIM_Base_Start_IT(hTIM);
#else

	// Enable DMA, DMA interrupts & peripherals
#ifdef CNC_USE_MULTI_BUFFER
	/* Set the DMA Period elapsed callback */
  hTIM->hdma[TIM_DMA_ID_UPDATE]->XferCpltCallback = TIM_DMAPeriodElapsedCplt;
     
  /* Set the DMA error callback */
  hTIM->hdma[TIM_DMA_ID_UPDATE]->XferErrorCallback = HAL_TIM_DMAError ;
	
	/* Enable the DMA Multibuffer Stream */
	HAL_DMAEx_MultiBufferStart_IT(hTIM->hdma[TIM_DMA_ID_UPDATE], (uint32_t)pAxis->dmaBuffer, (uint32_t)hTIM->Instance->ARR, (uint32_t)pAxis->dmaBuffer1, steps);
	
	/* Enable the TIM Update DMA request */
  __HAL_TIM_ENABLE_DMA(hTIM, TIM_DMA_UPDATE);

  /* Enable the Peripheral */
  __HAL_TIM_ENABLE(hTIM);
#endif

	HAL_TIM_Base_Start_DMA(hTIM, pAxis->dmaBuffer, steps);
#endif
	
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_Init_Masked(CNC_AxisTypeDef axisType, uint8_t mask)
{
	if (((1 << axisType) & mask) != 0)	return CNC_Axis_Init(axisType);
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_Prepare_Masked(CNC_AxisTypeDef axisType, uint8_t mask, uint32_t time, int32_t position)
{
	if (((1 << axisType) & mask) != 0) return CNC_Axis_Prepare(axisType, time, position);
	return HAL_OK;
}

HAL_StatusTypeDef CNC_Axis_Start_Masked(CNC_AxisTypeDef axisType, uint8_t mask, uint32_t time, int32_t position)
{
	if (((1 << axisType) & mask) != 0) return CNC_Axis_Start(axisType, time, position);
	return HAL_OK;
}

uint32_t CalculateMask(uint32_t steps, uint32_t ord)
{
	uint32_t mask = 0;
	uint32_t A = steps;
	uint32_t B = ord+1;

	int offset = 1;
	uint32_t p = 1;
	uint32_t q = 2;

	// mask calculation algorithm
	while ((B != 0) && (A != 0))
	{
		uint32_t Y = steps - (offset - 1);
		uint32_t SUB = Y / q;
		if ((Y % q) != 0)
			SUB += 1;

		if (B > SUB)
		{
			B -= SUB;
			mask |= p;
		}

		offset <<= 1;
		A >>= 1;
		p <<= 1;
		q <<= 1;
	}

	return mask;
}

