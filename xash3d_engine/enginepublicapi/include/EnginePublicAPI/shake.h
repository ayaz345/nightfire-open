/***
 *
 *	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *	This product contains software technology licensed from Id
 *	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
 *	All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC.  All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 ****/

#pragma once

#include "PlatformDefs/typedefs.h"

#define FFADE_IN 0x0000  // Just here so we don't pass 0 into the function
#define FFADE_OUT 0x0001  // Fade out (not in)
#define FFADE_MODULATE 0x0002  // Modulate (don't blend)
#define FFADE_STAYOUT 0x0004  // ignores the duration, stays faded out until new ScreenFade message received
#define FFADE_LONGFADE 0x0008  // used to indicate the fade can be longer than 16 seconds (added for czero)

// screen shake
extern int gmsgShake;

// Fade in/out
extern int gmsgFade;

// This structure is sent over the net to describe a screen shake event
typedef struct
{
	unsigned short amplitude;  // FIXED 4.12 amount of shake
	unsigned short duration;  // FIXED 4.12 seconds duration
	unsigned short frequency;  // FIXED 8.8 noise frequency (low frequency is a jerk,high frequency is a rumble)
} ScreenShake;

// This structure is sent over the net to describe a screen fade event
typedef struct
{
	unsigned short duration;  // FIXED 4.12 seconds duration
	unsigned short holdTime;  // FIXED 4.12 seconds duration until reset (fade & hold)
	short fadeFlags;  // flags
	byte r, g, b, a;  // fade to color ( max alpha )
} ScreenFade;
