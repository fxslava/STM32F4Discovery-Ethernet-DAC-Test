#include "cnc_machine.h"
#include "cnc_laser.h"
#include "cnc_axises.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_dac.h"

uint32_t GetUpdatePeriod(float period_ms)
{
	// calculate update period
	uint32_t PCLK1 = HAL_RCC_GetPCLK1Freq(); //22020096;
	uint32_t period = (uint32_t)floorf((float)PCLK1 * period_ms);
	return period;
}

void InitializeCNCMachine(CNC_MACHINE* machine, osThreadId tid_Thread)
{	
	// calculate step length
	machine->dx = 0.1f;
	machine->dy = 0.1f;
	machine->dz = 0.1f;
	machine->da = 0.1f;
	machine->_dx = 1.0f / machine->dx;
	machine->_dy = 1.0f / machine->dy;
	machine->_dz = 1.0f / machine->dz;
	machine->_da = 1.0f / machine->da;
	
	// set CNC thread id
	machine->tid_Thread_CNC = tid_Thread;
	
	// Update rate
	machine->period_ms = 0.001f;
	machine->period = GetUpdatePeriod(machine->period_ms);
	
	// Base parameters
	machine->x = 0;
	machine->y = 0;
	machine->z = 0;
	machine->a = 0;
	machine->position = CNCVec4Set(0.0f, 0.0f, 0.0f, 0.0f);
	
	// Track controller init
	machine->init.acceleration = 1000.0f;
	machine->init.velocity_start = 0.0f;
	machine->init.velocity_end = 0.0f;
	machine->init.velocity_max = 1000.0f;
	machine->init.start = CNCVec4Set(0.0f, 0.0f, 0.0f, 0.0f);
	machine->init.end = CNCVec4Set(0.0f, 0.0f, 0.0f, 0.0f);
	
	// setup laser
	machine->config.maxPower = 1.0f;
	machine->config.minPower = 0.3f;
	
	// Initialize track
	InitLineTrajectory(&machine->init, &machine->trajectory);
	InitArcTrajectory(&machine->init, &machine->trajectoryArc, CNCVec4Set(0.0f, 0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, WORK_PLANE_XY);
	
	// Initialize axises
	CNC_Axis_Init(CNC_X_AXIS);
	CNC_Axis_Init(CNC_Y_AXIS);
	CNC_Axis_Init(CNC_Z_AXIS);
	CNC_Axis_Init(CNC_A_AXIS);
}

void MoveTo(CNC_MACHINE* machine, int8_t axises, CNC_VECTOR4 position)
{	
	CNC_VECTOR4 point;
	float time = 0.0f;
	float total_time = 0.0f;
	float period_ms = machine->period_ms;
	float _dx = machine->_dx;
	float _dy = machine->_dy;
	float _dz = machine->_dz;
	float _da = machine->_da;
	uint32_t period = machine->period;
	
	// set new position (moveXY)
	machine->init.start = machine->position;
	machine->init.end = position;
	
	// calculate trajectory
	InitLineTrajectory(&machine->init, &machine->trajectory);
	DACC1(UpdateLineTrajectory(&machine->trajectory, &point, 0.0f), machine);
	
	if ((axises & AXIS_X_EVENT) != 0)	machine->x = (int32_t)(point.x * _dx);
	if ((axises & AXIS_Y_EVENT) != 0)	machine->y = (int32_t)(point.y * _dy);
	if ((axises & AXIS_Z_EVENT) != 0)	machine->z = (int32_t)(point.z * _dz);
	if ((axises & AXIS_A_EVENT) != 0)	machine->a = (int32_t)(point.a * _da);
	
	CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, period, machine->x);
	CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, period, machine->y);
	CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, period, machine->z);
	CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, period, machine->a);
	
	total_time = machine->trajectory.helper_curve.time[3];
	for (time = 0.0f; time < total_time; time += period_ms)
	{		
		if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
		if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
		if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
		if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
		DACC1(UpdateLineTrajectory(&machine->trajectory, &point, time), machine);
		
		if ((axises & AXIS_X_EVENT) != 0)	machine->x = (int32_t)(point.x * _dx);
		if ((axises & AXIS_Y_EVENT) != 0)	machine->y = (int32_t)(point.y * _dy);
		if ((axises & AXIS_Z_EVENT) != 0)	machine->z = (int32_t)(point.z * _dz);
		if ((axises & AXIS_A_EVENT) != 0)	machine->a = (int32_t)(point.a * _da);
		
		CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, period, machine->x);
		CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, period, machine->y);
		CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, period, machine->z);
		CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, period, machine->a);
	
		osThreadYield();	// suspend thread
	
		// Wait for axises
		//osSignalWait(axises, osWaitForever);
		while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);
	}
	
	if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
	if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
	if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
	if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
	point = machine->init.end;
	
	machine->x = (int32_t)(point.x * _dx);
	machine->y = (int32_t)(point.y * _dy);
	machine->z = (int32_t)(point.z * _dz);
	machine->a = (int32_t)(point.a * _da);	
	
	CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, period, machine->x);
	CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, period, machine->y);
	CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, period, machine->z);
	CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, period, machine->a);
	
	osThreadYield();	// suspend thread
	
	// Wait for axises
	//osSignalWait(axises, osWaitForever);
	while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);

	if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
	if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
	if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
	if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
	osThreadYield();	// suspend thread
	
	// Wait for axises
	//osSignalWait(axises, osWaitForever);
	while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);
	
	// set machine position
	machine->position = point;
}

void ArcTo(CNC_MACHINE* machine, int8_t axises, CNC_VECTOR4 center, float radius, float angle1, float angle2)
{	
	CNC_VECTOR4 point;
	float time = 0.0f;
	float total_time = 0.0f;
	float period_ms = machine->period_ms;
	float _dx = machine->_dx;
	float _dy = machine->_dy;
	float _dz = machine->_dz;
	float _da = machine->_da;
	uint32_t period = machine->period;
	
	// calculate trajectory
	InitArcTrajectory(&machine->init, &machine->trajectoryArc, center, radius, angle1, angle2, WORK_PLANE_XY);
	UpdateArcTrajectory(&machine->trajectoryArc, &point, 0.0f);
	
	machine->x = (int32_t)(point.x * _dx);
	machine->y = (int32_t)(point.y * _dy);
	machine->z = (int32_t)(point.z * _dz);
	machine->a = (int32_t)(point.a * _da);
	
	CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, period, machine->x);
	CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, period, machine->y);
	CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, period, machine->z);
	CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, period, machine->a);
	
	total_time = machine->trajectoryArc.helper_curve.time[3];
	for (time = 0.0f; time < total_time; time += period_ms)
	{
		if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
		if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
		if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
		if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
		UpdateArcTrajectory(&machine->trajectoryArc, &point, time);
	
		if ((axises & AXIS_X_EVENT) != 0)	machine->x = (int32_t)(point.x * _dx);
		if ((axises & AXIS_Y_EVENT) != 0)	machine->y = (int32_t)(point.y * _dy);
		if ((axises & AXIS_Z_EVENT) != 0)	machine->z = (int32_t)(point.z * _dz);
		if ((axises & AXIS_A_EVENT) != 0)	machine->a = (int32_t)(point.a * _da);

		CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, period, machine->x);
		CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, period, machine->y);
		CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, period, machine->z);
		CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, period, machine->a);
	
		osThreadYield();	// suspend thread
	
		// Wait for axises
		//osSignalWait(axises, osWaitForever);
		while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);
	}
	if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
	if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
	if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
	if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
	point = machine->init.end;
	
	machine->x = (int32_t)(point.x * _dx);
	machine->y = (int32_t)(point.y * _dy);
	machine->z = (int32_t)(point.z * _dz);
	machine->a = (int32_t)(point.a * _da);	

	CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, period, machine->x);
	CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, period, machine->y);
	CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, period, machine->z);
	CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, period, machine->a);
	
	osThreadYield();	// suspend thread
	
	// Wait for axises
	//osSignalWait(axises, osWaitForever);
	while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);

	if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
	if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
	if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
	if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
	osThreadYield();	// suspend thread
	
	// Wait for axises
	//osSignalWait(axises, osWaitForever);
	while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);
	
	// set machine position
	machine->position = point;
}

void MoveToAdaptive(CNC_MACHINE* machine, uint8_t axises, CNC_VECTOR4 position)
{	
	CNC_VECTOR4 point;
	CNC_VECTOR4 time4 = CNCVec4Set(0.0f, 0.0f, 0.0f, 0.0f);
	float _dx = machine->_dx;
	float _dy = machine->_dy;
	float _dz = machine->_dz;
	float _da = machine->_da;
	uint8_t Axises = 0;
	CNC_TIME_QUANTIZER quantizer;
	
	// set new position (moveXY)
	machine->init.start = machine->position;
	machine->init.end = position;
	
	// calculate trajectory
	InitLineTrajectory(&machine->init, &machine->trajectory);
	// init quantizer
	InitializeTimeQuantizer(&quantizer, &machine->trajectory, 1.0f);
	// calculate axis control points
	IndependentUpdateLineTrajectory(&machine->trajectory, &point, &quantizer.time_stp);
	// update position
	machine->x = (int32_t)(point.x * _dx);
	machine->y = (int32_t)(point.y * _dy);
	machine->z = (int32_t)(point.z * _dz);
	machine->a = (int32_t)(point.a * _da);
	// prepare axises
	CNC_Axis_Prepare_Masked(CNC_X_AXIS, axises, quantizer.periodx, machine->x);
	CNC_Axis_Prepare_Masked(CNC_Y_AXIS, axises, quantizer.periody, machine->y);
	CNC_Axis_Prepare_Masked(CNC_Z_AXIS, axises, quantizer.periodz, machine->z);
	CNC_Axis_Prepare_Masked(CNC_A_AXIS, axises, quantizer.perioda, machine->a);
	// start axises
	if ((axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
	if ((axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
	if ((axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
	if ((axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	
	while (IncrementTimeQuantizer(&quantizer, &time4, &Axises) == QUANTIZER_PROCESS)
	{
		IndependentUpdateLineTrajectory(&machine->trajectory, &point, &time4);
		
		if ((Axises & axises & AXIS_X_EVENT) != 0)	machine->x = (int32_t)(point.x * _dx);
		if ((Axises & axises & AXIS_Y_EVENT) != 0)	machine->y = (int32_t)(point.y * _dy);
		if ((Axises & axises & AXIS_Z_EVENT) != 0)	machine->z = (int32_t)(point.z * _dz);
		if ((Axises & axises & AXIS_A_EVENT) != 0)	machine->a = (int32_t)(point.a * _da);

		CNC_Axis_Prepare_Masked(CNC_X_AXIS, Axises & axises, quantizer.periodx, machine->x);
		CNC_Axis_Prepare_Masked(CNC_Y_AXIS, Axises & axises, quantizer.periody, machine->y);
		CNC_Axis_Prepare_Masked(CNC_Z_AXIS, Axises & axises, quantizer.periodz, machine->z);
		CNC_Axis_Prepare_Masked(CNC_A_AXIS, Axises & axises, quantizer.perioda, machine->a);
		
		osThreadYield();	// suspend thread
		
		// Wait for axises
		//osSignalWait(Axises & axises, osWaitForever);
		//while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);
		while (((CNC_XAxis.status & 0x2) != 0x2) && ((Axises & 0x1) != 0));
		while (((CNC_YAxis.status & 0x2) != 0x2) && ((Axises & 0x2) != 0));
		
		if ((Axises & axises & AXIS_X_EVENT) != 0)	CNC_Axis_StartX();
		if ((Axises & axises & AXIS_Y_EVENT) != 0)	CNC_Axis_StartY();
		if ((Axises & axises & AXIS_Z_EVENT) != 0)	CNC_Axis_StartZ();
		if ((Axises & axises & AXIS_A_EVENT) != 0)	CNC_Axis_StartA();
	}
	
	// Wait for all axises
	//osSignalWait(Axises & axises, osWaitForever);
	//while ((CNC_XAxis.status & CNC_YAxis.status & 0x2) != 0x2);
	while (((CNC_XAxis.status & 0x2) != 0x2) && ((Axises & 0x1) != 0));
	while (((CNC_YAxis.status & 0x2) != 0x2) && ((Axises & 0x2) != 0));
	
	// set machine position
	machine->position = point;
}

void MoveToXY(CNC_MACHINE* machine, float X, float Y)
{
	// active axises
	int8_t axis_signals = 0x3;
	
	// slow version
	MoveTo(machine, axis_signals, CNCVec4Set(X, Y, machine->z, machine->a));
}

typedef struct
{
	uint16_t	div;
	float			step;
} CNC_QSTEP;

CNC_QSTEP qstep(uint16_t	div,	float	step)
{
	CNC_QSTEP res;	
	res.div = div;
	res.step = step;
	return res;
}

int16_t max(int16_t x, int16_t y)
{
	if (x > y)
		return x;
	else
		return y;
}

CNC_QSTEP qmin(CNC_QSTEP x, CNC_QSTEP y)
{
	if (x.div < y.div)
		return x;
	else
		return y;
}

void InitializeTimeQuantizer(CNC_TIME_QUANTIZER* quantizer, CNC_TRAJECTORY* trajectory, float min_discr_step)
{
	CNC_VECTOR4 point;
	CNC_VECTOR4 base_point;
	float total_time = trajectory->helper_curve.time[3];
	
	quantizer->total_time = total_time;
	quantizer->divx = 1;
	quantizer->divy = 1;
	quantizer->divz = 1;
	quantizer->diva = 1;
	
	UpdateLineTrajectory(trajectory, &point, total_time);
	base_point = point;
	while (point.x > min_discr_step) 
	{
		quantizer->divx <<= 1;
		UpdateLineTrajectory(trajectory, &point, total_time / quantizer->divx);
	}
	point = base_point;
	while (point.y > min_discr_step) 
	{
		quantizer->divy <<= 1;
		UpdateLineTrajectory(trajectory, &point, total_time / quantizer->divy);
	}
	point = base_point;
	while (point.z > min_discr_step) 
	{
		quantizer->divz <<= 1;
		UpdateLineTrajectory(trajectory, &point, total_time / quantizer->divz);
	}
	point = base_point;
	while (point.a > min_discr_step) 
	{
		quantizer->diva <<= 1;
		UpdateLineTrajectory(trajectory, &point, total_time / quantizer->diva);
	}
	
	quantizer->time_stp.x = total_time / quantizer->divx;
	quantizer->time_stp.y = total_time / quantizer->divy;
	quantizer->time_stp.z = total_time / quantizer->divz;
	quantizer->time_stp.a = total_time / quantizer->diva;
	
	quantizer->cnt = 0;
	
	quantizer->period = max(max(quantizer->divx, quantizer->divy), max(quantizer->divz, quantizer->diva));
	
	quantizer->periodx = GetUpdatePeriod(quantizer->time_stp.x);
	quantizer->periody = GetUpdatePeriod(quantizer->time_stp.y);
	quantizer->periodz = GetUpdatePeriod(quantizer->time_stp.z);
	quantizer->perioda = GetUpdatePeriod(quantizer->time_stp.a);
	
	quantizer->divx = quantizer->period / quantizer->divx;
	quantizer->divy = quantizer->period / quantizer->divy;
	quantizer->divz = quantizer->period / quantizer->divz;
	quantizer->diva = quantizer->period / quantizer->diva;
						
	//quantizer->time_stp = total_time / quantizer->period;
	
	quantizer->cnt = 0;
}

CNC_TIME_QUANTIZER_STATE IncrementTimeQuantizer(CNC_TIME_QUANTIZER* quantizer, CNC_VECTOR4* time, uint8_t *axises)
{
	// Update counter
	quantizer->cnt ++;
	if (quantizer->cnt > quantizer->period)
		quantizer->cnt = quantizer->period;
	
	// Update time
	*time = CNCVec4Add(*time, quantizer->time_stp);
	
	// All axis down
	*axises = 0;
	
	// Restart axises
	if ((quantizer->cnt % quantizer->divx) == 0) *axises |= 0x1;
	if ((quantizer->cnt % quantizer->divy) == 0) *axises |= 0x2;
	if ((quantizer->cnt % quantizer->divz) == 0) *axises |= 0x4;
	if ((quantizer->cnt % quantizer->diva) == 0) *axises |= 0x8;
	
	if (quantizer->cnt == quantizer->period) 
	{
		time->x = quantizer->total_time;
		time->y = quantizer->total_time;
		time->z = quantizer->total_time;
		time->a = quantizer->total_time;
	
		*axises = 0xf;
		
		return QUANTIZER_STOP;
	}
	else
		return QUANTIZER_PROCESS;
}

uint32_t GetCommandParamCount(uint32_t cmd)
{
	uint16_t i = 0;
	uint16_t m = 0x0010;
	uint16_t n = 0;
	
	for (i = 0; i < 10; i++)
	{
		if ((cmd & m) != 0) n++;
		m <<= 1;
	}
	
	return n;
}
