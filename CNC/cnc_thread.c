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

/*----------------------------------------------------------------------------
 *      Thread 2 'Thread_CNC': Sample thread
 *---------------------------------------------------------------------------*/
 
CNC_MACHINE CNC_Machine;

uint8_t		readEnable;
uint8_t		writeEnable;
uint8_t		bufferReadTskPos;
uint8_t		bufferWriteTskPos;
int32_t		receivedTskCount;
int32_t		perfomedTskCount;

volatile	CNC_TASK	cncTaskBuffer[CMD_BUFFER_SIZE];

volatile ARM_DRIVER_USART * USARTdrv;
 
void Thread_CNC (void const *argument);                             // thread function
osThreadId tid_Thread_CNC;                                          // thread id
osThreadDef (Thread_CNC, osPriorityNormal, 1, 0);                   // thread object

int Init_Thread_CNC (void) {
	
  tid_Thread_CNC = osThreadCreate (osThread(Thread_CNC), NULL);
  if(!tid_Thread_CNC) return(-1);
  
  return(0);
}


void ProcessCmdGold(CNC_MACHINE* CNC_Machine)
{
	CNC_VECTOR4 position = CNC_Machine->position;
	CNC_VECTOR4 center = position;
	uint32_t cmd = 0;
	
	CNC_Receive(&cmd, 4);

	switch (cmd & 0x000f)
		{
			case CNC_CMD_LINE_MOVE:
				{
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_MOVE_X, &position.x);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_MOVE_Y, &position.y);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_MOVE_Z, &position.z);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_MOVE_A, &position.a);
					
					// LASER ON/OFF bits
					if ((cmd & CNC_CMD_LASER_OFF) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
					if ((cmd & CNC_CMD_LASER_ON) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
					
					// Move if axises is eneabled
					MoveTo(CNC_Machine, (cmd & CNC_CMD_MOVE_XY) >> 4, position);
				}
				break;
			case CNC_CMD_ARC_MOVE:
				{
					float Radius;
					float Angle1;
					float Angle2;
					// Circle for XYZ axises only
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ARC_CENTER_X, &center.x);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ARC_CENTER_Y, &center.y);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ARC_CENTER_Z, &center.z);
					Radius = 0.0f;
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ARC_CENTER_R, &Radius);
					Angle1 = 0.0f;								// Default zero angle point start position
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ARC_ANGLE1, 	&Angle1);
					Angle2 = PI * 2.0f + Angle1;	// Draw circle if angle param is single
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ARC_ANGLE2, 	&Angle2);
					
					InitArcTrajectory(&CNC_Machine->init, &CNC_Machine->trajectoryArc, center, Radius, Angle1, Angle2, WORK_PLANE_XY);
					// Move if axises is eneabled
					MoveTo(CNC_Machine, CNC_CMD_MOVE_XY >> 4, CNC_Machine->trajectoryArc.start); // Move to first angle position
					
					// LASER ON/OFF bits
					if ((cmd & CNC_CMD_LASER_OFF) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
					if ((cmd & CNC_CMD_LASER_ON) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
					
					// Move if axises is eneabled
					ArcTo (CNC_Machine, CNC_CMD_MOVE_XY >> 4, center, Radius, Angle1, Angle2); // Move to first angle position
				}
				break;
			case CNC_CMD_SET_SETTINGS:
				{
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_START_VELOCITY, &CNC_Machine->init.velocity_start);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_END_VELOCITY,   &CNC_Machine->init.velocity_end);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_MAX_VELOCITY,   &CNC_Machine->init.velocity_max);
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ACCELERATION,   &CNC_Machine->init.acceleration);
					
					// LASER ON/OFF bits
					if ((cmd & CNC_CMD_LASER_OFF) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
					if ((cmd & CNC_CMD_LASER_ON) != 0)	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
				}
				break;
			case CNC_CMD_QUALITY:
				{
					__PROCESS_CNC_CMD_PARAMF(cmd, CNC_CMD_ELEMENT_PERIOD, &CNC_Machine->period_ms);
					CNC_Machine->period = GetUpdatePeriod(CNC_Machine->period_ms);
				}
				break;
		}
}

void QueueCmd(CNC_MACHINE* CNC_Machine)
{
	CNC_TASK		task = {0};
	uint16_t size;
	
	CNC_Receive(&task.cmd, 4);						// Asynchronous receive command

	task.param_cnt = GetCommandParamCount(task.cmd);

	CNC_Receive(&task.param[0], 4 * task.param_cnt);
	
	cncTaskBuffer[bufferWriteTskPos++] = task;
	
	bufferWriteTskPos = bufferWriteTskPos & CMD_BUFFER_MASK;
	
	receivedTskCount++;
	
	size = receivedTskCount - perfomedTskCount;
	
	if (size >= CMD_BUFFER_SIZE)
		writeEnable = 0;	// disable write to queue
	
	if (readEnable == 0)
	if (size != 0)
		readEnable = 1;		// enable read from queue
}

void Thread_CNC (void const *argument) 
{	
	uint32_t cmd = 0;
	
	// current command
	writeEnable = 1;
	readEnable = 0;
	receivedTskCount = 0;
	perfomedTskCount = 0;
	bufferWriteTskPos = 0;
	bufferReadTskPos = 0;
	
	// initialize communication interface
	USARTdrv = USART_Init(tid_Thread_CNC);
	CNC_LaserPort_Init();
	
	// initialize machine
	InitializeCNCMachine(&CNC_Machine, tid_Thread_CNC);
	
  while (1) {
		ProcessCmdGold(&CNC_Machine);
		//QueueCmd(&CNC_Machine);		// Translate command
		
		while (writeEnable == 0)	osThreadYield();
			
		cmd = 0xfcfc;						// ACK
		CNC_Transmit(&cmd, 1);	// Asynchronous send
		
		osThreadYield();
  }
}


