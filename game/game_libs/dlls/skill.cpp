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
//=========================================================
// skill.cpp - code for skill level concerns
//=========================================================
#include "extdll.h"
#include "util.h"
#include "skill.h"
#include "PlatformLib/String.h"

skilldata_t gSkillData;

//=========================================================
// take the name of a cvar, tack a digit for the skill level
// on, and return the value.of that Cvar
//=========================================================
float GetSkillCvar(const char* pName)
{
	float flValue;
	char szBuffer[64];

	PlatformLib_SNPrintF(szBuffer, sizeof(szBuffer), "%s%d", pName, gSkillData.iSkillLevel);

	flValue = CVAR_GET_FLOAT(szBuffer);

	if ( flValue <= 0 )
	{
		ALERT(
			at_warning,
			"GetSkillCVar: Got invalid value %f for %s. Make sure the cvar exists and that its value value is "
			"correct.\n",
			flValue,
			szBuffer);
	}

	return flValue;
}
