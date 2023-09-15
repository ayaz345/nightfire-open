#pragma once

#include "PlatformDefs/typedefs.h"
#include "MathLib/mathdefs.h"

typedef vec_t vec4_t[4];
typedef vec_t quat_t[4];
typedef vec_t matrix3x4[3][4];
typedef vec_t matrix4x4[4][4];

typedef enum
{
	PITCH = 0,
	YAW,
	ROLL
} EulerAngleAxis_e;

// 0 - 2 are axial planes
// 3 needs alternate calc
typedef enum
{
	PLANE_X = 0,
	PLANE_Y,
	PLANE_Z,
	PLANE_NONAXIAL
} PlaneAxis_e;
