#include <stdint.h>
#include "cnc_def.h"

extern void InitHelperTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_HELPER_TRAJECTORY* pTrajectory, float pathLength);
extern void UpdateHelperTrajectory(CNC_HELPER_TRAJECTORY* pTrajectory, float* pos, float* velocity, float time);

extern void InitLineTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_TRAJECTORY* pTrajectory);
extern float UpdateLineTrajectory(CNC_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, float time);
extern void IndependentUpdateLineTrajectory(CNC_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, CNC_VECTOR4* time);

extern void InitArcTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_ARC_TRAJECTORY* pTrajectory, CNC_VECTOR4 center, float radius, float angle1, float angle2, WORK_PLANE plane);
extern float UpdateArcTrajectory(CNC_ARC_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, float time);

extern void InitArc3DTrajectory(CNC_TRAJECTORY_INIT* pInit, CNC_ARC3D_TRAJECTORY* pTrajectory, CNC_VECTOR4 center, CNC_VECTOR4 normal, float radius);
extern float UpdateArc3DTrajectory(CNC_ARC3D_TRAJECTORY* pTrajectory, CNC_VECTOR4* pos, float time);
