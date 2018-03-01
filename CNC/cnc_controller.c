#include <math.h>
#include <cmsis_os.h>
#include "cnc_math.h"
#include "cnc_controller.h"

extern osThreadId tid_Thread_CNC; 

/* ************************************************************************ */
/* ********************* Helpper Interpolation **************************** */
/* ************************************************************************ */

void InitHelperTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_HELPER_TRAJECTORY* pTrajectory, float pathLength)
{
	// trajectory temporary variables
	register float x0 = 0.0f;
	register float x1;				// not defined
	register float x2;				// not defined
	register float x3 = pathLength; //CNCVecDistance(pInit->end, pInit->start);
	register float t0 = 0.0f;
	register float t1;				// not defined
	register float t2;				// not defined
	register float t3;				// not defined
	
	// trajectory second order temporary variables
	register float L = x3;
	register float A = pInit->acceleration;
	register float v0 = pInit->velocity_start;
	register float v1 = pInit->velocity_end;
	register float vmax = pInit->velocity_max;
	register float vmid;			// not defined
	
	if (((2.0f * vmax * vmax - v0 * v0 - v1 * v1) * 0.5f / A) < L)
	{
		float _A = 1.0f / A;
		float treg = L/vmax - ((vmax - v0) * (vmax + v0) + (vmax - v1) * (vmax + v1)) / (2.0f * A * vmax);
		float tacc = (vmax - v0) * _A;
		float tdec = (vmax - v1) * _A;
		x1 = v0 * tacc + A * tacc * tacc * 0.5f;
		x2 = x1 + vmax * treg;
		t1 = tacc;
		t2 = t1 + treg;
		t3 = t2 + tdec;
		vmid = vmax;
	} else
	{
		float _A = 1.0f / A;
		float tacc = (sqrtf(A * L + (v0 * v0 + v1 * v1) * 0.5f) - v0) * _A;
    x1 = v0 * tacc + A * tacc * tacc * 0.5f;
    x2 = x1;
    t1 = tacc;
    t2 = t1;
    t3 = t2 + tacc + (v0 - v1) * _A;
    vmid = v0 + A * tacc;
	}
	
	// ----------------------------------------------------------
	// Helper curve parameters
	// ----------------------------------------------------------
	pTrajectory->acceleration = A;
	// Time track
	pTrajectory->time[0] = t0;
	pTrajectory->time[1] = t1;
	pTrajectory->time[2] = t2;
	pTrajectory->time[3] = t3;
	// Velocity track
	pTrajectory->v[0] = v0;
	pTrajectory->v[1] = vmid;
	pTrajectory->v[2] = vmid;
	pTrajectory->v[3] = v1;
	pTrajectory->vmid = vmid;
	// Position track
	pTrajectory->x[0] = x0;
	pTrajectory->x[1] = x1;
	pTrajectory->x[2] = x2;
	pTrajectory->x[3] = x3;
}

void UpdateHelperTrajectory(CNC_HELPER_TRAJECTORY* pTrajectory, float* pos, float* velocity, float time)
{
	float x;
	float v;
	
	// helper coords
	float L			= pTrajectory->x[3];
	float A			= pTrajectory->acceleration;
	float v0		= pTrajectory->v[0];
	float v1		= pTrajectory->v[3];
	float vmax	= pTrajectory->vmid;
	float vmid	= pTrajectory->vmid;
	float x1		= pTrajectory->x[1];
	float x2		= pTrajectory->x[2];
	float t1		= pTrajectory->time[1];
	float t2		= pTrajectory->time[2];
	float t3		= pTrajectory->time[3];
	
  if (time < t1) 
	{
    x = v0 * time + (A * time * time) * 0.5f;
    v = v0 + A * time;
  } else
  if (time < t2)
  {
    x = x1 + vmax * (time - t1);
    v = vmax;
  } else
  if (time < t3)
  {
		float tmp = (time - t2);
    v = vmid - A * (time - t2);
    x = x2 + vmid * (time - t2) - (A * tmp * tmp) * 0.5f;
  } else
  {
    x = L;
    v = v1;
  }
	
	*pos = x;
	*velocity = v;
}

/* ************************************************************************ */
/* ********************* Linear Interpolation ***************************** */
/* ************************************************************************ */

void InitLineTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_TRAJECTORY* pTrajectory)
{	
	// Calculate normal of move direction
	CNC_VECTOR4 dir = CNCVec4Subtract(pInit->end, pInit->start);
	CNC_VECTOR4 normal;
	
	float length = CNCVec4Length(dir); 
	if (length != 0.0f)
		normal = CNCVec4MultiplyScalar(dir, 1.0f / CNCVec4Length(dir));
	
	pTrajectory->normal = normal;
	pTrajectory->start = pInit->start;
	
	InitHelperTrajectory(pInit, &pTrajectory->helper_curve, CNCVec4Distance(pInit->end, pInit->start));
}

float UpdateLineTrajectory(CNC_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, float time)
{
	float x;
	float v;
	
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x, &v, time);
	
	*pos = CNCVec4Add(CNCVec4MultiplyScalar(pTrajectory->normal, x), pTrajectory->start);
	
	return v;
}

void IndependentUpdateLineTrajectory(CNC_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, CNC_VECTOR4* time)
{
	CNC_VECTOR4 x;
	CNC_VECTOR4 v;
	
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x.x, &v.x, time->x);
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x.y, &v.y, time->y);
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x.z, &v.z, time->z);
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x.a, &v.a, time->a);
	
	*pos = CNCVec4Add(CNCVec4Multiply(pTrajectory->normal, x), pTrajectory->start);
}

/* ************************************************************************ */
/* ********************* Arc Interpolation ******************************** */
/* ************************************************************************ */

void InitArcTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_ARC_TRAJECTORY* pTrajectory, CNC_VECTOR4 center, float radius, float angle1, float angle2, WORK_PLANE plane)
{
	pTrajectory->Angle1 = angle1;
	pTrajectory->Angle2 = angle2;
	pTrajectory->Radius = radius;
	pTrajectory->center = center;
	pTrajectory->plane  = plane;
	
	// XY, define start, end points
	switch (plane)
	{
		case WORK_PLANE_XY:
			pTrajectory->start.x = center.x + radius * cos(angle1);
			pTrajectory->start.y = center.y + radius * sin(angle1);
			pTrajectory->start.z = center.z;
			pTrajectory->start.a = center.a;
			
			pTrajectory->end.x = center.x + radius * cos(angle2);
			pTrajectory->end.y = center.y + radius * sin(angle2);
			pTrajectory->end.z = center.z;
			pTrajectory->end.a = center.a;
			break;
		case WORK_PLANE_XZ:
			pTrajectory->start.x = center.x + radius * cos(angle1);
			pTrajectory->start.y = center.y;
			pTrajectory->start.z = center.z + radius * sin(angle1);
			pTrajectory->start.a = center.a;
			
			pTrajectory->end.x = center.x + radius * cos(angle2);
			pTrajectory->end.y = center.y;
			pTrajectory->end.z = center.z + radius * sin(angle2);
			pTrajectory->end.a = center.a;
			break;
		case WORK_PLANE_ZY:
			pTrajectory->start.x = center.x;
			pTrajectory->start.y = center.y + radius * sin(angle1);
			pTrajectory->start.z = center.z + radius * cos(angle1);
			pTrajectory->start.a = center.a;
			
			pTrajectory->end.x = center.x;
			pTrajectory->end.y = center.y + radius * sin(angle2);
			pTrajectory->end.z = center.z + radius * cos(angle2);
			pTrajectory->end.a = center.a;
			break;
	}
	
	pInit->start = pTrajectory->start;
	pInit->end = pTrajectory->end;
	
	InitHelperTrajectory(pInit, &pTrajectory->helper_curve, (angle2 - angle1) * radius);
}

float UpdateArcTrajectory(CNC_ARC_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, float time)
{
	float x;
	float v;
	
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x, &v, time);

	x /= pTrajectory->Radius;
	x += pTrajectory->Angle1;
	
	switch (pTrajectory->plane)
	{
		case WORK_PLANE_XY:
			pos->x = pTrajectory->center.x + pTrajectory->Radius * cos(x);
			pos->y = pTrajectory->center.y + pTrajectory->Radius * sin(x);
			pos->z = pTrajectory->center.z + pTrajectory->center.z;
			pos->a = pTrajectory->center.a + pTrajectory->center.a;
		break;
		case WORK_PLANE_XZ:
			pos->x = pTrajectory->center.x + pTrajectory->Radius * cos(x);
			pos->y = pTrajectory->center.y + pTrajectory->center.y;
			pos->z = pTrajectory->center.z + pTrajectory->Radius * sin(x);
			pos->a = pTrajectory->center.a + pTrajectory->center.a;
		break;
		case WORK_PLANE_ZY:
			pos->x = pTrajectory->center.x + pTrajectory->center.x;
			pos->y = pTrajectory->center.y + pTrajectory->Radius * cos(x);
			pos->z = pTrajectory->center.z + pTrajectory->Radius * sin(x);
			pos->a = pTrajectory->center.a + pTrajectory->center.a;
		break;
	}
	
	return v;
}

/* ************************************************************************ */
/* ********************* Arc 3D Interpolation ***************************** */
/* ************************************************************************ */
void InitArc3DTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_ARC3D_TRAJECTORY* pTrajectory, CNC_VECTOR4 center, CNC_VECTOR4 normal, float radius)
{
	CNC_VECTOR4 Ex = CNCVec4Normalize(CNCVec4Subtract(pInit->start, center));
	CNC_VECTOR4 Ey = CNCVec4Normalize(CNCVec4Cross(Ex, normal));
	CNC_VECTOR4 Ez = CNCVec4Normalize(CNCVec4Subtract(pInit->end, center));
	CNC_VECTOR4 REx = CNCVec4MultiplyScalar(Ex, radius);
	CNC_VECTOR4 REy = CNCVec4MultiplyScalar(Ey, radius);
	
	//float phi0 = 0.0f;
	float phi1 = acosf(CNCVec4Dot(Ex, Ez));
	float _radius = 1.0f / radius;
	
	pTrajectory->REx = REx;
	pTrajectory->REy = REy;
	pTrajectory->center = center;
	pTrajectory->Radius = radius;
	pTrajectory->_Radius = _radius;
	
	InitHelperTrajectory(pInit, &pTrajectory->helper_curve, phi1 * radius);
}

CNC_VECTOR4 CalculateArcPos(CNC_VECTOR4 center, CNC_VECTOR4 REx, CNC_VECTOR4 REy, float _radius, float radius, float x)
{
	CNC_VECTOR4 ex = CNCVec4MultiplyScalar(REx, cosf(x * _radius));
	CNC_VECTOR4 ey = CNCVec4MultiplyScalar(REy, sinf(x * _radius));
	CNC_VECTOR4 basis = CNCVec4Add(ex, ey);
	CNC_VECTOR4 res = CNCVec4Add(center, basis);
	return res;
}

float UpdateArc3DTrajectory(CNC_ARC3D_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, float time)
{
	float x;
	float v;
	
	UpdateHelperTrajectory(&pTrajectory->helper_curve, &x, &v, time);
	
	*pos = CalculateArcPos(pTrajectory->center, pTrajectory->REx, pTrajectory->REy, pTrajectory->_Radius, pTrajectory->Radius, x);
	
	return v;
}

