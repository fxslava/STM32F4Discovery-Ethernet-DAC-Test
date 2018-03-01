#include <cmsis_os.h>  
#include <stdint.h>

//CMSIS
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_uart.h"

#include "Driver_USART.h"
#include "cnc_math.h"

#ifndef __CNC_INTERFACE_HH
#define __CNC_INTERFACE_HH

#define USART_TX_COMPLETE_EVENT	0x10
#define USART_RX_COMPLETE_EVENT	0x20
#define CMD_BUFFER_SIZE	32
#define CMD_BUFFER_MASK	31
//#define CMD_LENGTH 256

#define __PROCESS_CNC_CMD_PARAMF(cmd, mask, param) 				\
	if ((cmd & mask) != 0)																	\
	{																												\
		CNC_Receive(param, 4);																\
	}

typedef union
{
	float		fP;
	int32_t	iP;
}	CNC_PARAM;

typedef struct
{
	int32_t		cmd;
	int32_t		param_cnt;
	CNC_PARAM	param[10];
} CNC_TASK;

extern USART_HandleTypeDef hUSART6;
extern UART_HandleTypeDef hUART6;

extern ARM_DRIVER_USART* USART_Init(osThreadId tid_Thread);

extern void CNC_Receive(void* data, uint32_t num);
extern void CNC_Transmit(void* data, uint32_t num);

#endif
