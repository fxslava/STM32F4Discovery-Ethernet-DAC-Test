#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_tim.h"
#include "DACFIFO.h"
#include "DACModule.h"
#include "CNCLerp.h"
#include "FB3def.h"
#include <string.h>

extern bool			sendACK;					// send state
extern uint16_t	ackWord;					// global ack packet
extern eDACFIFO_t  fifo;					// FIFO

DAC_HandleTypeDef g_hDAC;
TIM_HandleTypeDef g_hTIMDAC;

#define TIMDAC_MEM0	false
#define TIMDAC_MEM1	true
	
// Lerp
bool startLerp = false;
bool bUpdate = true;
bool bCNCinit = false;
eFB3Lerp_t DACLerp;
eCNCLerp_t CNCLerp;
eFB3FastLerp_t DACFastLerp;

// TIM DAC param for mem0
int16_t x0;
int16_t y0;
int16_t dx0;
int16_t dy0;
uint16_t period0; // mem0

// TIM DAC param for mem1
int16_t x1;
int16_t y1;
int16_t dx1;
int16_t dy1;
uint16_t period1; // mem1

// TIM DAC counter
uint16_t cnt;

bool tim_state = TIMDAC_MEM0;
bool updateTimState = false;

void InitFastLerp(eFB3FastLerp_t* lerp, uint16_t period_exp, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	lerp->x1 = x1;
	lerp->y1 = y1;
	lerp->x2 = x2;
	lerp->y2 = y2;
	lerp->period_exp = period_exp;
	lerp->dirx = (lerp->x2 > lerp->x1) ? 1 : 0;
	lerp->diry = (lerp->y2 > lerp->y1) ? 1 : 0;
	lerp->dx = (((lerp->dirx == 1) ? (lerp->x2 - lerp->x1) : (lerp->x1 - lerp->x2)) >> lerp->period_exp);
	lerp->dy = (((lerp->diry == 1) ? (lerp->y2 - lerp->y1) : (lerp->y1 - lerp->y2)) >> lerp->period_exp);
}

eVector2 CalcFastLerp(eFB3FastLerp_t* lerp, bool *update)
{
	eVector2 pos;
	pos.x = lerp->x1;
	pos.y = lerp->y1;
	
	// calculate Lerp
	if (lerp->dirx == 1)	lerp->x1 += lerp->dx;
	else									lerp->x1 -= lerp->dx;
	if (lerp->diry == 1)	lerp->y1 += lerp->dy;
	else									lerp->y1 -= lerp->dy;
	
	// calculate update period
	*update = true;
	if(((lerp->dirx == 1) && (lerp->x1 < lerp->x2)) ||
		 ((lerp->dirx == 0) && (lerp->x1 > lerp->x2)))
		*update = false;
	else
		lerp->x1 = lerp->x2;
	if(((lerp->diry == 1) && (lerp->y1 < lerp->y2)) ||
		 ((lerp->diry == 0) && (lerp->y1 > lerp->y2)))	
		*update = false;
	else
		lerp->y1 = lerp->y2;
	
	return pos;
}

void UpdateFastLerp(eFB3FastLerp_t* lerp, int16_t x, int16_t y)
{
	lerp->x2 = x;
	lerp->y2 = y;
	lerp->dirx = (lerp->x2 > lerp->x1) ? 1 : 0;
	lerp->diry = (lerp->y2 > lerp->y1) ? 1 : 0;
	lerp->dx = (((lerp->dirx == 1) ? (lerp->x2 - lerp->x1) : (lerp->x1 - lerp->x2)) >> lerp->period_exp);
	lerp->dy = (((lerp->diry == 1) ? (lerp->y2 - lerp->y1) : (lerp->y1 - lerp->y2)) >> lerp->period_exp);
}

void InitLerp(eFB3Lerp_t* lerp, uint16_t period_exp, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	lerp->x1 = (x1 << 16);
	lerp->y1 = (y1 << 16);
	lerp->x2 = (x2 << 16);
	lerp->y2 = (y2 << 16);
	lerp->period_exp = period_exp;
	lerp->dirx = (lerp->x2 > lerp->x1) ? 1 : 0;
	lerp->diry = (lerp->y2 > lerp->y1) ? 1 : 0;
	lerp->dx = (((lerp->dirx == 1) ? (lerp->x2 - lerp->x1) : (lerp->x1 - lerp->x2)) >> lerp->period_exp);
	lerp->dy = (((lerp->diry == 1) ? (lerp->y2 - lerp->y1) : (lerp->y1 - lerp->y2)) >> lerp->period_exp);
}

eVector2 CalcLerp(eFB3Lerp_t* lerp, bool *update)
{
	eVector2 pos;
	pos.x = (lerp->x1 >> 16);
	pos.y = (lerp->y1 >> 16);
	
	// calculate Lerp
	if (lerp->dirx == 1)	lerp->x1 += lerp->dx;
	else									lerp->x1 -= lerp->dx;
	if (lerp->diry == 1)	lerp->y1 += lerp->dy;
	else									lerp->y1 -= lerp->dy;
	
	// calculate update period
	*update = true;
	if(((lerp->dirx == 1) && (lerp->x1 < lerp->x2)) ||
		 ((lerp->dirx == 0) && (lerp->x1 > lerp->x2)))
		*update = false;
	else
		lerp->x1 = lerp->x2;
	if(((lerp->diry == 1) && (lerp->y1 < lerp->y2)) ||
		 ((lerp->diry == 0) && (lerp->y1 > lerp->y2)))	
		*update = false;
	else
		lerp->y1 = lerp->y2;
	
	return pos;
}

void UpdateLerp(eFB3Lerp_t* lerp, int16_t x, int16_t y)
{
	lerp->x2 = (x << 16);
	lerp->y2 = (y << 16);
	lerp->dirx = (lerp->x2 > lerp->x1) ? 1 : 0;
	lerp->diry = (lerp->y2 > lerp->y1) ? 1 : 0;
	lerp->dx = (((lerp->dirx == 1) ? (lerp->x2 - lerp->x1) : (lerp->x1 - lerp->x2)) >> lerp->period_exp);
	lerp->dy = (((lerp->diry == 1) ? (lerp->y2 - lerp->y1) : (lerp->y1 - lerp->y2)) >> lerp->period_exp);
}

void InitDAC(void)
{
	DAC_ChannelConfTypeDef configChannel = {0};
	GPIO_InitTypeDef DACPort = {0};
	TIM_Base_InitTypeDef DACTIMconfig = {0};
	
	__GPIOA_CLK_ENABLE();
	
	// Analog pin
	DACPort.Pin				= GPIO_PIN_4 | GPIO_PIN_5; // Laser ON/OFF
	DACPort.Mode			= GPIO_MODE_ANALOG;
	DACPort.Pull			= GPIO_NOPULL;
	
	// Initialize port
	HAL_GPIO_Init(GPIOA, &DACPort);
	
	// Init DAC
	g_hDAC.Instance = DAC;
	RCC->APB1ENR |= RCC_APB1ENR_DACEN; // Surprise!!! ;)
	HAL_DAC_Init(&g_hDAC);
	
	// Config channel 1
	configChannel.DAC_Trigger = DAC_TRIGGER_NONE;
	configChannel.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	
	HAL_DAC_ConfigChannel(&g_hDAC, &configChannel, DAC_CHANNEL_1);
	HAL_DAC_ConfigChannel(&g_hDAC, &configChannel, DAC_CHANNEL_2);
	
	// Always on DAC
	HAL_DAC_Start(&g_hDAC, DAC_CHANNEL_1);
	HAL_DAC_Start(&g_hDAC, DAC_CHANNEL_2);
	
	// Set default
	HAL_DAC_SetValue(&g_hDAC, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x777);
	HAL_DAC_SetValue(&g_hDAC, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 0x777);
	
	// Initialize DAC Timer
	__TIM2_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	
	DACTIMconfig.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	DACTIMconfig.CounterMode = TIM_COUNTERMODE_UP;
	DACTIMconfig.Period = 0x0080;
	DACTIMconfig.Prescaler = 0x0001;
	
	g_hTIMDAC.Instance = TIM2;
	g_hTIMDAC.Init = DACTIMconfig;
	
	// initialize timer
	HAL_TIM_Base_Init(&g_hTIMDAC);
	
	// start timer
	//HAL_TIM_Base_Start_IT(&g_hTIMDAC);
	
	// init interpolator
	InitLerp(&DACLerp, 8, 0, 0, 0, 0);
	//InitFastLerp(&DACFastLerp, 2, 0, 0, 0, 0);
	
	CNCLerp.AxisMask[0] = 0;
	CNCLerp.AxisMask[1] = 0;
	CNCLerp.counter = 0;
	CNCLerp.period = 256;
	bUpdate = true;
	bCNCinit = false;
}

void DeInitDAC(void)
{
	HAL_DAC_Stop(&g_hDAC, DAC_CHANNEL_1);
	HAL_DAC_DeInit(&g_hDAC);
}

#define LERP_ENABLE

void ProcessDACFIFO(void)
{
	eOutCoord_t data;
	eVector2 vec;
	
	if (!startLerp) return;
	
/*
#ifdef LERP_ENABLE
	// Get data form fifo
	if (bUpdate && GetDataFastFromDACFIFO(&fifo, (char*)&data, sizeof(eOutCoord_t)))
		UpdateLerp(&DACLerp, data.x, data.y);
	
	// Calculate vector
	vec = CalcLerp(&DACLerp, &bUpdate);
	
	HAL_DAC_SetValue(&g_hDAC, DAC_CHANNEL_1, DAC_ALIGN_12B_R, vec.x);
	HAL_DAC_SetValue(&g_hDAC, DAC_CHANNEL_2, DAC_ALIGN_12B_R, vec.y);
#else
	if (GetDataFromDACFIFO(&fifo, (char*)&data, sizeof(eOutCoord_t)))
	{
		// DAC
		HAL_DAC_SetValue(&g_hDAC, DAC_CHANNEL_1, DAC_ALIGN_12B_R, data.x);
		HAL_DAC_SetValue(&g_hDAC, DAC_CHANNEL_2, DAC_ALIGN_12B_R, data.y);
	}
#endif*/

// Get data form fifo
	if (bUpdate && GetDataFastFromDACFIFO(&fifo, (char*)&data, sizeof(eOutCoord_t)))
	{
		CNCLerp.AxisMask[0] = data.cur;
		CNCLerp.AxisMask[1] = data.focus;
		CNCLerp.AxisMask[2] = data.feed;
		bCNCinit = true;
	}
	
	if (bCNCinit)
		UpdateCNCLerp(&CNCLerp, 256, &bUpdate);
}


