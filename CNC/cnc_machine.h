#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include <stdint.h>
// CNC
#include "cnc_math.h"
#include "cnc_controller.h"

#ifndef __CNC_MACHINE_HH
#define __CNC_MACHINE_HH

// CNC command definitions
#define CNC_CMD_NOP							0x0000	// Nop
#define CNC_CMD_LINE_MOVE				0x0001	// Line interpolation move for all axises
#define CNC_CMD_ARC_MOVE				0x0002	// Arc interpolation move for working plane
#define CNC_CMD_SPLINE_MOVE			0x0003	// Spline interpolation move for working plane
#define CNC_CMD_ARC3D_MOVE			0x0004	// Arc interpolation in 3D
#define CNC_CMD_SPLINE3D_MOVE		0x0005	// Spline interpolation in 3D
#define CNC_CMD_SET_SETTINGS		0x0006	// Set settings
#define CNC_CMD_GET_SETTINGS		0x0007	// Get settings
#define CNC_CMD_LOAD_PROGRAM		0x0008	// Load program block to device memory
#define CNC_CMD_CALL_PROGRAM		0x0009	// Call program from device memory
#define CNC_CMD_LOOP_PROGRAM		0x000a	// Loop program
#define CNC_CMD_SET_CONDTITION	0x000b	// Set loop condition
#define CNC_CMD_GET_CONDTITION	0x000c	// Get loop condition
#define CNC_CMD_SET_PID					0x000d	// Set PID
#define CNC_CMD_GET_PID					0x000e	// Get PID
#define CNC_CMD_QUALITY					0x000f	// Ping device

// Line move
#define CNC_CMD_MOVE_X					0x0010	// move one axis
#define CNC_CMD_MOVE_Y					0x0020
#define CNC_CMD_MOVE_Z					0x0040
#define CNC_CMD_MOVE_A					0x0080
#define CNC_CMD_MOVE_XY					0x0030	// move axis plane
#define CNC_CMD_MOVE_XZ					0x0050
#define CNC_CMD_MOVE_YZ					0x0060
#define CNC_CMD_MOVE_XYZ				0x0070	// move space

// Arc move
#define CNC_CMD_ARC_CENTER_X		0x0010	// arc set center axis coords
#define CNC_CMD_ARC_CENTER_Y		0x0020
#define CNC_CMD_ARC_CENTER_Z		0x0040
#define CNC_CMD_ARC_CENTER_R		0x0080
#define CNC_CMD_ARC_CENTER_XY		0x0030	// arc axis planes
#define CNC_CMD_ARC_CENTER_XZ		0x0050
#define CNC_CMD_ARC_CENTER_YZ		0x0060
#define CNC_CMD_ARC_CENTER_XYZ	0x0070
#define CNC_CMD_ARC_XYZR				0x00f0	// center & radius
#define CNC_CMD_ARC_ANGLE1			0x0100	// first arc angle
#define CNC_CMD_ARC_ANGLE2			0x0200	// second arc angle

//Quality
#define CNC_CMD_ELEMENT_PERIOD	0x0010

// Settings
#define CNC_CMD_START_VELOCITY	0x0400	// move constants
#define CNC_CMD_END_VELOCITY		0x0800
#define CNC_CMD_MAX_VELOCITY		0x1000
#define CNC_CMD_ACCELERATION		0x2000
#define CNC_CMD_MOVE_SETTINGS		0x3c00

// Laser on/off
#define CNC_CMD_LASER_ON				0x4000	// laser
#define CNC_CMD_LASER_OFF				0x8000

typedef struct __CNC_MACHINE
{
	// CNC thread id
	osThreadId tid_Thread_CNC;
	
	// head location
	CNC_VECTOR4 position;
	
	// laser
	float last_power;
	CNC_LASER_CONFIG config;
	
	// actual head location
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t a;
	
	// step length (mm)
	float dx;
	float dy;
	float dz;
	float da;
	
	// step length (mm)
	float _dx;
	float _dy;
	float _dz;
	float _da;
	
	// update rate
	float period_ms;
	uint32_t period;
	
	// controller
	CNC_TRAJECTORY_INIT init;
	CNC_TRAJECTORY trajectory;
	CNC_ARC_TRAJECTORY trajectoryArc;
} CNC_MACHINE;

typedef struct __CNC_TIME_QUANTIZER
{
	CNC_VECTOR4 time_stp;
	
	uint16_t divx;
	uint16_t divy;
	uint16_t divz;
	uint16_t diva;
	
	uint32_t periodx;
	uint32_t periody;
	uint32_t periodz;
	uint32_t perioda;
	
	int cnt;
	int period;
	float total_time;
} CNC_TIME_QUANTIZER;

typedef enum
{
	QUANTIZER_PROCESS = 0,
	QUANTIZER_STOP = 1
} CNC_TIME_QUANTIZER_STATE;

typedef enum
{
	CNC_MODE_LINEAR = 0,
	CNC_MODE_ARC = 1
} CNC_INTERPOLATION_MODE;

extern uint32_t GetUpdatePeriod(float period_ms);
extern void InitializeCNCMachine(CNC_MACHINE* machine, osThreadId tid_Thread);
extern void MoveTo(CNC_MACHINE* machine, int8_t axises, CNC_VECTOR4 position);
extern void ArcTo(CNC_MACHINE* machine, int8_t axises, CNC_VECTOR4 center, float radius, float angle1, float angle2);
extern void MoveToAdaptive(CNC_MACHINE* machine, uint8_t axises, CNC_VECTOR4 position);
extern void MoveToXY(CNC_MACHINE* machine, float X, float Y);

extern void InitializeTimeQuantizer(CNC_TIME_QUANTIZER* quantizer, CNC_TRAJECTORY* trajectory, float min_discr_step);
extern CNC_TIME_QUANTIZER_STATE IncrementTimeQuantizer(CNC_TIME_QUANTIZER* quantizer, CNC_VECTOR4* time, uint8_t *axises);

extern uint32_t GetCommandParamCount(uint32_t cmd);

#endif
