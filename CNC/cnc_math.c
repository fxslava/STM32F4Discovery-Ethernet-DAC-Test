#include "cnc_math.h"

CNC_VECTOR4 CNCVec4Set(float x, float y, float z, float a)
{
	CNC_VECTOR4 point;
	point.x = x;
	point.y = y;
	point.z = z;
	point.a = a;
	return point;
}

CNC_VECTOR3 CNCVec3Set(float x, float y, float z)
{
	CNC_VECTOR3 point;
	point.x = x;
	point.y = y;
	point.z = z;
	return point;
}

CNC_VECTOR2 CNCVec2Set(float x, float y)
{
	CNC_VECTOR2 point;
	point.x = x;
	point.y = y;
	return point;
}

CNC_VECTOR4 CNCVec4MultiplyScalar(CNC_VECTOR4 p, float t)
{
	CNC_VECTOR4 res;
	res.x = p.x * t;
	res.y = p.y * t;
	res.z = p.z * t;
	res.a = p.a * t;
	return res;
}

CNC_VECTOR3 CNCVec3MultiplyScalar(CNC_VECTOR3 p, float t)
{
	CNC_VECTOR3 res;
	res.x = p.x * t;
	res.y = p.y * t;
	res.z = p.z * t;
	return res;
}

CNC_VECTOR2 CNCVec2MultiplyScalar(CNC_VECTOR2 p, float t)
{
	CNC_VECTOR2 res;
	res.x = p.x * t;
	res.y = p.y * t;
	return res;
}

CNC_VECTOR4 CNCVec4Multiply(CNC_VECTOR4 p, CNC_VECTOR4 t)
{
	CNC_VECTOR4 res;
	res.x = p.x * t.x;
	res.y = p.y * t.y;
	res.z = p.z * t.z;
	res.a = p.a * t.a;
	return res;
}

CNC_VECTOR3 CNCVec3Multiply(CNC_VECTOR3 p, CNC_VECTOR3 t)
{
	CNC_VECTOR3 res;
	res.x = p.x * t.x;
	res.y = p.y * t.y;
	res.z = p.z * t.z;
	return res;
}

CNC_VECTOR2 CNCVec2Multiply(CNC_VECTOR2 p, CNC_VECTOR2 t)
{
	CNC_VECTOR2 res;
	res.x = p.x * t.x;
	res.y = p.y * t.y;
	return res;
}

CNC_VECTOR4 CNCVec4Cross(CNC_VECTOR4 p, CNC_VECTOR4 t)
{
	CNC_VECTOR4 res;
	res.x = p.y*t.z - p.z*t.y;
	res.y = p.z*t.x - p.x*t.z;
	res.z = p.x*t.y - p.y*t.x;
	res.a = p.a * t.a;
	return res;
}

CNC_VECTOR3 CNCVec3Cross(CNC_VECTOR3 p, CNC_VECTOR3 t)
{
	CNC_VECTOR3 res;
	res.x = p.y*t.z - p.z*t.y;
	res.y = p.z*t.x - p.x*t.z;
	res.z = p.x*t.y - p.y*t.x;
	return res;
}

CNC_VECTOR4 CNCVec4Subtract(CNC_VECTOR4 p, CNC_VECTOR4 t)
{
	CNC_VECTOR4 res;
	res.x = p.x - t.x;
	res.y = p.y - t.y;
	res.z = p.z - t.z;
	res.a = p.a - t.a;
	return res;
}

CNC_VECTOR3 CNCVec3Subtract(CNC_VECTOR3 p, CNC_VECTOR3 t)
{
	CNC_VECTOR3 res;
	res.x = p.x - t.x;
	res.y = p.y - t.y;
	res.z = p.z - t.z;
	return res;
}

CNC_VECTOR2 CNCVec2Subtract(CNC_VECTOR2 p, CNC_VECTOR2 t)
{
	CNC_VECTOR2 res;
	res.x = p.x - t.x;
	res.y = p.y - t.y;
	return res;
}

CNC_VECTOR4 CNCVec4Add(CNC_VECTOR4 p, CNC_VECTOR4 t)
{
	CNC_VECTOR4 res;
	res.x = p.x + t.x;
	res.y = p.y + t.y;
	res.z = p.z + t.z;
	res.a = p.a + t.a;
	return res;
}

CNC_VECTOR3 CNCVec3Add(CNC_VECTOR3 p, CNC_VECTOR3 t)
{
	CNC_VECTOR3 res;
	res.x = p.x + t.x;
	res.y = p.y + t.y;
	res.z = p.z + t.z;
	return res;
}

CNC_VECTOR2 CNCVec2Add(CNC_VECTOR2 p, CNC_VECTOR2 t)
{
	CNC_VECTOR2 res;
	res.x = p.x + t.x;
	res.y = p.y + t.y;
	return res;
}

float CNCVec4Dot(CNC_VECTOR4 p, CNC_VECTOR4 t)
{
	return p.x * t.x + p.y * t.y + p.z * t.z + p.a * t.a;
}

float CNCVec3Dot(CNC_VECTOR3 p, CNC_VECTOR3 t)
{
	return p.x * t.x + p.y * t.y + p.z * t.z;
}

float CNCVec2Dot(CNC_VECTOR2 p, CNC_VECTOR2 t)
{
	return p.x * t.x + p.y * t.y;
}

float CNCVec4Length(CNC_VECTOR4 p)
{
	return sqrtf(CNCVec4Dot(p, p));
}

float CNCVec3Length(CNC_VECTOR3 p)
{
	return sqrtf(CNCVec3Dot(p, p));
}

float CNCVec2Length(CNC_VECTOR2 p)
{
	return sqrtf(CNCVec2Dot(p, p));
}

float CNCVec4Distance(CNC_VECTOR4 p, CNC_VECTOR4 t)
{
	return CNCVec4Length(CNCVec4Subtract(p, t));;
}

float CNCVec3Distance(CNC_VECTOR3 p, CNC_VECTOR3 t)
{
	return CNCVec3Length(CNCVec3Subtract(p, t));;
}

float CNCVec2Distance(CNC_VECTOR2 p, CNC_VECTOR2 t)
{
	return CNCVec2Length(CNCVec2Subtract(p, t));;
}

CNC_VECTOR4 CNCVec4Normalize(CNC_VECTOR4 p)
{
	return CNCVec4MultiplyScalar(p, 1.0f/CNCVec4Length(p));
}

CNC_VECTOR3 CNCVec3Normalize(CNC_VECTOR3 p)
{
	return CNCVec3MultiplyScalar(p, 1.0f/CNCVec3Length(p));
}

CNC_VECTOR2 CNCVec2Normalize(CNC_VECTOR2 p)
{
	return CNCVec2MultiplyScalar(p, 1.0f/CNCVec2Length(p));
}
