#include <cmsis_os.h>                                           // CMSIS RTOS header file
// CNC
#include "cnc_interface.h"
// USART Driver
#include "Driver_USART.h"

extern ARM_DRIVER_USART Driver_USART6;
osThreadId tid_Thread_Receiver; 

void USART_Gcode_callback(uint32_t event)
{	
	switch (event)
	{
		case ARM_USART_EVENT_RECEIVE_COMPLETE:
				/* Success: Wakeup Thread */
				osSignalSet(tid_Thread_Receiver, USART_RX_COMPLETE_EVENT);
		case ARM_USART_EVENT_TRANSFER_COMPLETE:
		case ARM_USART_EVENT_SEND_COMPLETE:
		case ARM_USART_EVENT_TX_COMPLETE:
				/* Success: Wakeup Thread */
				osSignalSet(tid_Thread_Receiver, USART_TX_COMPLETE_EVENT);
			break;
		case ARM_USART_EVENT_RX_TIMEOUT:
				__breakpoint(0); /* Error: Call debugger or replace with custom error handling */
			break;
		case ARM_USART_EVENT_RX_OVERFLOW:
		case ARM_USART_EVENT_TX_UNDERFLOW:
				__breakpoint(0); /* Error: Call debugger or replace with custom error handling */
			break;
	}
}

ARM_DRIVER_USART* USART_Init(osThreadId tid_Thread)
{
	ARM_DRIVER_USART * USARTdrv = 0;
	uint32_t status = ARM_DRIVER_OK;
	USARTdrv = &Driver_USART6;
	
#ifdef DEBUG
	ARM_USART_CAPABILITIES drv_capabilities;
	ARM_DRIVER_VERSION version;
	
	version = USARTdrv->GetVersion();
	if (version.api < 0x200) /* requires at minimum API version 2.00 or higher */
	{ /* error handling */
		return;
	}
	drv_capabilities = USARTdrv->GetCapabilities();
	if (drv_capabilities.event_tx_complete == 0)
	{ /* error handling */
		return;
	}
#endif
	/* Set thread id for callback */
	tid_Thread_Receiver = tid_Thread;
	/*Initialize the USART driver */
	USARTdrv->Initialize(USART_Gcode_callback);
	/*Power up the USART peripheral */
	USARTdrv->PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 9600 Bits/sec */
	status = USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
										ARM_USART_DATA_BITS_8 |
										ARM_USART_PARITY_NONE |
										ARM_USART_STOP_BITS_1 |
										ARM_USART_FLOW_CONTROL_NONE, 9600);
	// USART init fail
	if (status != ARM_DRIVER_OK) __breakpoint(0);
	
	/* Enable Receiver and Transmitter lines */
	//USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
	
	return USARTdrv;
}
