#ifndef __CNC_MATH_HH
#define __CNC_MATH_HH

#include <stdint.h>
#include <math.h>

#define ARM_MATH_CM4
#define __FPU_PRESENT 1

#include <arm_math.h>

typedef struct
{
	float x;
	float y;
	float z;
	float a;
} CNC_VECTOR4;

typedef struct
{
	float x;
	float y;
	float z;
} CNC_VECTOR3;

typedef struct
{
	float x;
	float y;
} CNC_VECTOR2;

extern CNC_VECTOR4 CNCVec4Set(float x, float y, float z, float a);
extern CNC_VECTOR3 CNCVec3Set(float x, float y, float z);
extern CNC_VECTOR2 CNCVec2Set(float x, float y);

// vector operations
extern CNC_VECTOR4 CNCVec4MultiplyScalar(CNC_VECTOR4 p, float t);
extern CNC_VECTOR3 CNCVec3MultiplyScalar(CNC_VECTOR3 p, float t);
extern CNC_VECTOR2 CNCVec2MultiplyScalar(CNC_VECTOR2 p, float t);

extern CNC_VECTOR4 CNCVec4Multiply(CNC_VECTOR4 p, CNC_VECTOR4 t);
extern CNC_VECTOR3 CNCVec3Multiply(CNC_VECTOR3 p, CNC_VECTOR3 t);
extern CNC_VECTOR2 CNCVec2Multiply(CNC_VECTOR2 p, CNC_VECTOR2 t);

extern CNC_VECTOR4 CNCVec4Cross(CNC_VECTOR4 p, CNC_VECTOR4 t);
extern CNC_VECTOR3 CNCVec3Cross(CNC_VECTOR3 p, CNC_VECTOR3 t);

extern CNC_VECTOR4 CNCVec4Subtract(CNC_VECTOR4 p, CNC_VECTOR4 t);
extern CNC_VECTOR3 CNCVec3Subtract(CNC_VECTOR3 p, CNC_VECTOR3 t);
extern CNC_VECTOR2 CNCVec2Subtract(CNC_VECTOR2 p, CNC_VECTOR2 t);

extern CNC_VECTOR4 CNCVec4Add(CNC_VECTOR4 p, CNC_VECTOR4 t);
extern CNC_VECTOR3 CNCVec3Add(CNC_VECTOR3 p, CNC_VECTOR3 t);
extern CNC_VECTOR2 CNCVec2Add(CNC_VECTOR2 p, CNC_VECTOR2 t);

extern CNC_VECTOR4 CNCVec4Normalize(CNC_VECTOR4 p);
extern CNC_VECTOR3 CNCVec3Normalize(CNC_VECTOR3 p);
extern CNC_VECTOR2 CNCVec2Normalize(CNC_VECTOR2 p);

// scalar operations
extern float CNCVec4Dot(CNC_VECTOR4 p, CNC_VECTOR4 t);
extern float CNCVec3Dot(CNC_VECTOR3 p, CNC_VECTOR3 t);
extern float CNCVec2Dot(CNC_VECTOR2 p, CNC_VECTOR2 t);

extern float CNCVec4Length(CNC_VECTOR4 p);
extern float CNCVec3Length(CNC_VECTOR3 p);
extern float CNCVec2Length(CNC_VECTOR2 p);

extern float CNCVec4Distance(CNC_VECTOR4 p, CNC_VECTOR4 t);
extern float CNCVec3Distance(CNC_VECTOR3 p, CNC_VECTOR3 t);
extern float CNCVec2Distance(CNC_VECTOR2 p, CNC_VECTOR2 t);

#endif
