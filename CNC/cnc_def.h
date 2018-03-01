#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_tim.h"

#include "cnc_math.h"

#ifndef __CNC_DEFINE_HH
#define __CNC_DEFINE_HH

#define AXIS_X_EVENT 0x01
#define AXIS_Y_EVENT 0x02
#define AXIS_Z_EVENT 0x04
#define AXIS_A_EVENT 0x08

#define TIM_DMA_BUFFER_SIZE	128

typedef enum
{
	AXIS_FORWARD,
	AXIS_BACKWARD
} CNC_AXIS_DIRECTION;

typedef enum
{
	AXIS_BUSY = 0,
	AXIS_READY = 1,
	AXIS_STOP = 2
} CNC_AXIS_STATUS;

typedef enum
{
	WORK_PLANE_XY,
	WORK_PLANE_XZ,
	WORK_PLANE_ZY
} WORK_PLANE;

typedef enum
{
	AXIS_MEM0,
	AXIS_MEM1
} AXIS_CURR_MEM;

typedef struct __CNC_AXIS
{
	int32_t steps;
	AXIS_CURR_MEM mem;
	
	// TIM options
	uint32_t period;
	uint16_t prescale;
	uint16_t mask;
	uint32_t dmaBuffer [TIM_DMA_BUFFER_SIZE];
	uint32_t dmaBuffer1[TIM_DMA_BUFFER_SIZE];
	
	// ***
	uint32_t src_position;
	uint32_t dst_position;
	CNC_AXIS_DIRECTION direction;
	
	// axis status
	CNC_AXIS_STATUS status;
} CNC_AXIS;

typedef enum
{
	CNC_X_AXIS = 0,
	CNC_Y_AXIS = 1,
	CNC_Z_AXIS = 2,
	CNC_A_AXIS = 3
} CNC_AxisTypeDef;

typedef struct
{
	CNC_VECTOR4 start;
	CNC_VECTOR4 end;
	float velocity_start;
	float velocity_end;
	float velocity_max;
	float acceleration;
} CNC_TRAJECTORY_INIT;

typedef struct
{
	float time[4];
	float x[4];
	float v[4];
	float vmid;
	float acceleration;
} CNC_HELPER_TRAJECTORY;

// Line trajectory
typedef struct
{
	CNC_VECTOR4 normal;
	CNC_VECTOR4 start;
	CNC_HELPER_TRAJECTORY helper_curve;
} CNC_TRAJECTORY;

// Arc trajectory
typedef struct
{
	CNC_VECTOR4 normal;
	CNC_VECTOR4 start;
	CNC_VECTOR4 center;
	CNC_VECTOR4 REx;
	CNC_VECTOR4 REy;
	float _Radius;
	float Radius;
	CNC_HELPER_TRAJECTORY helper_curve;
} CNC_ARC3D_TRAJECTORY;

// Arc trajectory
typedef struct
{
	CNC_VECTOR4 center;
	CNC_VECTOR4 start;
	CNC_VECTOR4 end;
	float Radius;
	float Angle1;
	float Angle2;
	CNC_HELPER_TRAJECTORY helper_curve;
	WORK_PLANE plane;
} CNC_ARC_TRAJECTORY;

typedef struct __CNC_LASER_CONFIG
{
	float minPower;
	float maxPower;
} CNC_LASER_CONFIG;

#endif
