
#include <cmsis_os.h>                                           // CMSIS RTOS header file
// CNC
#include "cnc_laser.h"
#include "cnc_machine.h"
#include "cnc_interface.h"

// CMSIS
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_usart.h"

extern	uint8_t		readEnable;
extern	uint8_t		writeEnable;
extern	uint8_t		bufferReadTskPos;
extern	uint8_t		bufferWriteTskPos;
extern	int32_t		receivedTskCount;
extern	int32_t		perfomedTskCount;

extern	CNC_MACHINE CNC_Machine;

extern	CNC_TASK		cncTaskBuffer[CMD_BUFFER_SIZE];

#define __PROCESS_CNC_CMD_IDX_PARAMF(task, mask, p, index) 	\
	if ((task.cmd & mask) != 0)																		\
	{																															\
		(p) = task.param[index++].fP;														\
	}	

#define __PROCESS_CNC_CMD_IDX_PARAMI(task, mask, p, index) 	\
	if ((task.cmd & mask) != 0)																		\
	{																															\
		(p) = task.param[index++].iP;														\
	}	

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
void Thread_TarsformCommand (void const *argument);                             // thread function
osThreadId tid_Thread_TarsformCommand;                                          // thread id
osThreadDef (Thread_TarsformCommand, osPriorityNormal, 1, 0);                   // thread object

int Init_TarsformCommand_Thread (void) {

  tid_Thread_TarsformCommand = osThreadCreate (osThread(Thread_TarsformCommand), NULL);
  if(!tid_Thread_TarsformCommand) return(-1);
  
  return(0);
}

void ProcessCmd(CNC_MACHINE* CNC_Machine, CNC_TASK task)
{
	CNC_VECTOR4 position = CNC_Machine->position;
	CNC_VECTOR4 center = position;
	uint8_t index = 0;
	
	switch (task.cmd & 0x000f)
		{
			case CNC_CMD_LINE_MOVE:
				{
					index = 0;
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_MOVE_X, position.x, index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_MOVE_Y, position.y, index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_MOVE_Z, position.z, index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_MOVE_A, position.a, index);
					
					// LASER ON/OFF bits
					if ((task.cmd & CNC_CMD_LASER_OFF) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
					if ((task.cmd & CNC_CMD_LASER_ON) != 0)		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
					
					// Move if axises is eneabled
					MoveTo(CNC_Machine, (task.cmd & CNC_CMD_MOVE_XY) >> 4, position);
				}
				break;
			case CNC_CMD_ARC_MOVE:
				{
					float Radius;
					float Angle1;
					float Angle2;
					index = 0;
					// Circle for XYZ axises only
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ARC_CENTER_X, center.x, index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ARC_CENTER_Y, center.y, index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ARC_CENTER_Z, center.z, index);
					Radius = 0.0f;
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ARC_CENTER_R, Radius, index);
					Angle1 = 0.0f;								// Default zero angle point start position
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ARC_ANGLE1, 	Angle1, index);
					Angle2 = PI * 2.0f + Angle1;	// Draw circle if angle param is single
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ARC_ANGLE2, 	Angle2, index);
					
					InitArcTrajectory(&CNC_Machine->init, &CNC_Machine->trajectoryArc, center, Radius, Angle1, Angle2, WORK_PLANE_XY);
					// Move if axises is eneabled
					MoveTo(CNC_Machine, CNC_CMD_MOVE_XY >> 4, CNC_Machine->trajectoryArc.start); // Move to first angle position
					
					// LASER ON/OFF bits
					if ((task.cmd & CNC_CMD_LASER_OFF) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
					if ((task.cmd & CNC_CMD_LASER_ON) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
					
					// Move if axises is eneabled
					ArcTo (CNC_Machine, CNC_CMD_MOVE_XY >> 4, center, Radius, Angle1, Angle2); // Move to first angle position
				}
				break;
			case CNC_CMD_SET_SETTINGS:
				{
					index = 0;
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_START_VELOCITY, CNC_Machine->init.velocity_start, index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_END_VELOCITY,   CNC_Machine->init.velocity_end  , index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_MAX_VELOCITY,   CNC_Machine->init.velocity_max  , index);
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ACCELERATION,   CNC_Machine->init.acceleration  , index);
					
					// LASER ON/OFF bits
					if ((task.cmd & CNC_CMD_LASER_OFF) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
					if ((task.cmd & CNC_CMD_LASER_ON) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
				}
				break;
			case CNC_CMD_QUALITY:
				{
					index = 0;
					__PROCESS_CNC_CMD_IDX_PARAMF(task, CNC_CMD_ELEMENT_PERIOD, CNC_Machine->period_ms, index);
					CNC_Machine->period = GetUpdatePeriod(CNC_Machine->period_ms);
				}
				break;
		}
}

void Thread_TarsformCommand (void const *argument) {
	CNC_TASK task;
	int16_t size;
	
  while (1) {
    if (readEnable == 1)
		{
			task = cncTaskBuffer[bufferReadTskPos++];
			
			ProcessCmd(&CNC_Machine, task);
			
			bufferReadTskPos = bufferReadTskPos & CMD_BUFFER_MASK;
	
			perfomedTskCount++;
			
			size = receivedTskCount - perfomedTskCount;
			
			if (size <= 0)
				readEnable = 0;		// disable read from queue
			
			if (writeEnable == 0)
			if (size <= (CMD_BUFFER_SIZE / 2))
				writeEnable = 1;		// enable write to queue
		}
    
		osThreadYield();                                            // suspend thread
  }
}


