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
#include "extdll.h"
#include "EnginePublicAPI/eiface.h"
#include "util.h"
#include "game.h"
#include "weaponregistry.h"
#include "bot_cvars.h"
#include "bot_commands.h"
#include "projectInterface/IProjectInterface.h"
#include "projectInterface_server.h"
#include "gameresources/GameResources.h"
#include "hitbox_debugging/hitbox_commands.h"
#include "customGeometry/commands.h"
#include "resources/SoundResources.h"
#include "gameplay/weaponInaccuracyCvars.h"

#define CREATE_CVAR(name, valstr, flags, valfloat, next) \
	{ \
		const_cast<char*>(name), const_cast<char*>(valstr), flags, valfloat, next \
	}

cvar_t displaysoundlist = CREATE_CVAR("displaysoundlist", "0", 0, 0.0f, nullptr);

// multiplayer server rules
cvar_t fragsleft = CREATE_CVAR(
	"mp_fragsleft",
	"0",
	FCVAR_SERVER | FCVAR_UNLOGGED,
	0.0f,
	nullptr);  // Don't spam console/log files/users with this changing
cvar_t timeleft = CREATE_CVAR("mp_timeleft", "0", FCVAR_SERVER | FCVAR_UNLOGGED, 0.0f, nullptr);

// multiplayer server rules
cvar_t teamplay = CREATE_CVAR("mp_teamplay", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t fraglimit = CREATE_CVAR("mp_fraglimit", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t timelimit = CREATE_CVAR("mp_timelimit", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t friendlyfire = CREATE_CVAR("mp_friendlyfire", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t falldamage = CREATE_CVAR("mp_falldamage", "1", FCVAR_SERVER, 1.0f, nullptr);
cvar_t weaponstay = CREATE_CVAR("mp_weaponstay", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t selfgauss = CREATE_CVAR("mp_selfgauss", "1", FCVAR_SERVER, 1.0f, nullptr);
cvar_t satchelfix = CREATE_CVAR("mp_satchelfix", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t forcerespawn = CREATE_CVAR("mp_forcerespawn", "1", FCVAR_SERVER, 1.0f, nullptr);
cvar_t flashlight = CREATE_CVAR("mp_flashlight", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t aimcrosshair = CREATE_CVAR("mp_autocrosshair", "1", FCVAR_SERVER, 1.0f, nullptr);
cvar_t decalfrequency = CREATE_CVAR("decalfrequency", "30", FCVAR_SERVER, 30.0f, nullptr);
cvar_t teamlist = CREATE_CVAR("mp_teamlist", "hgrunt;scientist", FCVAR_SERVER, 0.0f, nullptr);
cvar_t teamoverride = CREATE_CVAR("mp_teamoverride", "1", 0, 1.0f, nullptr);
cvar_t defaultteam = CREATE_CVAR("mp_defaultteam", "0", 0, 0.0f, nullptr);
cvar_t allowmonsters = CREATE_CVAR("mp_allowmonsters", "0", FCVAR_SERVER, 0.0f, nullptr);
cvar_t bhopcap = CREATE_CVAR("mp_bhopcap", "1", FCVAR_SERVER, 1.0f, nullptr);

// Avoid respawning players closer to where they last died than this.
cvar_t mp_respawn_avoid_radius = CREATE_CVAR("mp_respawn_avoid_radius", "192", FCVAR_SERVER, 192.0f, nullptr);
cvar_t mp_corpse_show_time = CREATE_CVAR("mp_corpse_show_time", "30", FCVAR_SERVER, 30.0f, nullptr);

cvar_t allow_spectators =
	CREATE_CVAR("allow_spectators", "1", FCVAR_SERVER, 1.0f, nullptr);  // 0 prevents players from being spectators
cvar_t multibyte_only = CREATE_CVAR("mp_multibyte_only", "0", FCVAR_SERVER, 0.0f, nullptr);

cvar_t mp_chattime = CREATE_CVAR("mp_chattime", "10", FCVAR_SERVER, 10.0f, nullptr);

// Engine Cvars
cvar_t* g_psv_gravity = NULL;
cvar_t* g_psv_aim = NULL;
cvar_t* g_footsteps = NULL;

// CVARS FOR SKILL LEVEL SETTINGS
//  Agrunt
cvar_t sk_agrunt_health1 = CREATE_CVAR("sk_agrunt_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_agrunt_health2 = CREATE_CVAR("sk_agrunt_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_agrunt_health3 = CREATE_CVAR("sk_agrunt_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_agrunt_dmg_punch1 = CREATE_CVAR("sk_agrunt_dmg_punch1", "0", 0, 0.0f, nullptr);
cvar_t sk_agrunt_dmg_punch2 = CREATE_CVAR("sk_agrunt_dmg_punch2", "0", 0, 0.0f, nullptr);
cvar_t sk_agrunt_dmg_punch3 = CREATE_CVAR("sk_agrunt_dmg_punch3", "0", 0, 0.0f, nullptr);

// Apache
cvar_t sk_apache_health1 = CREATE_CVAR("sk_apache_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_apache_health2 = CREATE_CVAR("sk_apache_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_apache_health3 = CREATE_CVAR("sk_apache_health3", "0", 0, 0.0f, nullptr);

// Barney
cvar_t sk_barney_health1 = CREATE_CVAR("sk_barney_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_barney_health2 = CREATE_CVAR("sk_barney_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_barney_health3 = CREATE_CVAR("sk_barney_health3", "0", 0, 0.0f, nullptr);

// Bullsquid
cvar_t sk_bullsquid_health1 = CREATE_CVAR("sk_bullsquid_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_health2 = CREATE_CVAR("sk_bullsquid_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_health3 = CREATE_CVAR("sk_bullsquid_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_bullsquid_dmg_bite1 = CREATE_CVAR("sk_bullsquid_dmg_bite1", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_dmg_bite2 = CREATE_CVAR("sk_bullsquid_dmg_bite2", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_dmg_bite3 = CREATE_CVAR("sk_bullsquid_dmg_bite3", "0", 0, 0.0f, nullptr);

cvar_t sk_bullsquid_dmg_whip1 = CREATE_CVAR("sk_bullsquid_dmg_whip1", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_dmg_whip2 = CREATE_CVAR("sk_bullsquid_dmg_whip2", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_dmg_whip3 = CREATE_CVAR("sk_bullsquid_dmg_whip3", "0", 0, 0.0f, nullptr);

cvar_t sk_bullsquid_dmg_spit1 = CREATE_CVAR("sk_bullsquid_dmg_spit1", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_dmg_spit2 = CREATE_CVAR("sk_bullsquid_dmg_spit2", "0", 0, 0.0f, nullptr);
cvar_t sk_bullsquid_dmg_spit3 = CREATE_CVAR("sk_bullsquid_dmg_spit3", "0", 0, 0.0f, nullptr);

// Big Momma
cvar_t sk_bigmomma_health_factor1 = CREATE_CVAR("sk_bigmomma_health_factor1", "1.0", 0, 1.0f, nullptr);
cvar_t sk_bigmomma_health_factor2 = CREATE_CVAR("sk_bigmomma_health_factor2", "1.0", 0, 1.0f, nullptr);
cvar_t sk_bigmomma_health_factor3 = CREATE_CVAR("sk_bigmomma_health_factor3", "1.0", 0, 1.0f, nullptr);

cvar_t sk_bigmomma_dmg_slash1 = CREATE_CVAR("sk_bigmomma_dmg_slash1", "50", 0, 50.0f, nullptr);
cvar_t sk_bigmomma_dmg_slash2 = CREATE_CVAR("sk_bigmomma_dmg_slash2", "50", 0, 50.0f, nullptr);
cvar_t sk_bigmomma_dmg_slash3 = CREATE_CVAR("sk_bigmomma_dmg_slash3", "50", 0, 50.0f, nullptr);

cvar_t sk_bigmomma_dmg_blast1 = CREATE_CVAR("sk_bigmomma_dmg_blast1", "100", 0, 100.0f, nullptr);
cvar_t sk_bigmomma_dmg_blast2 = CREATE_CVAR("sk_bigmomma_dmg_blast2", "100", 0, 100.0f, nullptr);
cvar_t sk_bigmomma_dmg_blast3 = CREATE_CVAR("sk_bigmomma_dmg_blast3", "100", 0, 100.0f, nullptr);

cvar_t sk_bigmomma_radius_blast1 = CREATE_CVAR("sk_bigmomma_radius_blast1", "250", 0, 250.0f, nullptr);
cvar_t sk_bigmomma_radius_blast2 = CREATE_CVAR("sk_bigmomma_radius_blast2", "250", 0, 250.0f, nullptr);
cvar_t sk_bigmomma_radius_blast3 = CREATE_CVAR("sk_bigmomma_radius_blast3", "250", 0, 250.0f, nullptr);

// Gargantua
cvar_t sk_gargantua_health1 = CREATE_CVAR("sk_gargantua_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_health2 = CREATE_CVAR("sk_gargantua_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_health3 = CREATE_CVAR("sk_gargantua_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_gargantua_dmg_slash1 = CREATE_CVAR("sk_gargantua_dmg_slash1", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_dmg_slash2 = CREATE_CVAR("sk_gargantua_dmg_slash2", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_dmg_slash3 = CREATE_CVAR("sk_gargantua_dmg_slash3", "0", 0, 0.0f, nullptr);

cvar_t sk_gargantua_dmg_fire1 = CREATE_CVAR("sk_gargantua_dmg_fire1", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_dmg_fire2 = CREATE_CVAR("sk_gargantua_dmg_fire2", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_dmg_fire3 = CREATE_CVAR("sk_gargantua_dmg_fire3", "0", 0, 0.0f, nullptr);

cvar_t sk_gargantua_dmg_stomp1 = CREATE_CVAR("sk_gargantua_dmg_stomp1", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_dmg_stomp2 = CREATE_CVAR("sk_gargantua_dmg_stomp2", "0", 0, 0.0f, nullptr);
cvar_t sk_gargantua_dmg_stomp3 = CREATE_CVAR("sk_gargantua_dmg_stomp3", "0", 0, 0.0f, nullptr);

// Hassassin
cvar_t sk_hassassin_health1 = CREATE_CVAR("sk_hassassin_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_hassassin_health2 = CREATE_CVAR("sk_hassassin_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_hassassin_health3 = CREATE_CVAR("sk_hassassin_health3", "0", 0, 0.0f, nullptr);

// Headcrab
cvar_t sk_headcrab_health1 = CREATE_CVAR("sk_headcrab_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_headcrab_health2 = CREATE_CVAR("sk_headcrab_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_headcrab_health3 = CREATE_CVAR("sk_headcrab_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_headcrab_dmg_bite1 = CREATE_CVAR("sk_headcrab_dmg_bite1", "0", 0, 0.0f, nullptr);
cvar_t sk_headcrab_dmg_bite2 = CREATE_CVAR("sk_headcrab_dmg_bite2", "0", 0, 0.0f, nullptr);
cvar_t sk_headcrab_dmg_bite3 = CREATE_CVAR("sk_headcrab_dmg_bite3", "0", 0, 0.0f, nullptr);

// Hgrunt
cvar_t sk_hgrunt_health1 = CREATE_CVAR("sk_hgrunt_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_health2 = CREATE_CVAR("sk_hgrunt_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_health3 = CREATE_CVAR("sk_hgrunt_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_hgrunt_kick1 = CREATE_CVAR("sk_hgrunt_kick1", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_kick2 = CREATE_CVAR("sk_hgrunt_kick2", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_kick3 = CREATE_CVAR("sk_hgrunt_kick3", "0", 0, 0.0f, nullptr);

cvar_t sk_hgrunt_pellets1 = CREATE_CVAR("sk_hgrunt_pellets1", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_pellets2 = CREATE_CVAR("sk_hgrunt_pellets2", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_pellets3 = CREATE_CVAR("sk_hgrunt_pellets3", "0", 0, 0.0f, nullptr);

cvar_t sk_hgrunt_gspeed1 = CREATE_CVAR("sk_hgrunt_gspeed1", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_gspeed2 = CREATE_CVAR("sk_hgrunt_gspeed2", "0", 0, 0.0f, nullptr);
cvar_t sk_hgrunt_gspeed3 = CREATE_CVAR("sk_hgrunt_gspeed3", "0", 0, 0.0f, nullptr);

// Houndeye
cvar_t sk_houndeye_health1 = CREATE_CVAR("sk_houndeye_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_houndeye_health2 = CREATE_CVAR("sk_houndeye_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_houndeye_health3 = CREATE_CVAR("sk_houndeye_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_houndeye_dmg_blast1 = CREATE_CVAR("sk_houndeye_dmg_blast1", "0", 0, 0.0f, nullptr);
cvar_t sk_houndeye_dmg_blast2 = CREATE_CVAR("sk_houndeye_dmg_blast2", "0", 0, 0.0f, nullptr);
cvar_t sk_houndeye_dmg_blast3 = CREATE_CVAR("sk_houndeye_dmg_blast3", "0", 0, 0.0f, nullptr);

// ISlave
cvar_t sk_islave_health1 = CREATE_CVAR("sk_islave_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_health2 = CREATE_CVAR("sk_islave_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_health3 = CREATE_CVAR("sk_islave_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_islave_dmg_claw1 = CREATE_CVAR("sk_islave_dmg_claw1", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_dmg_claw2 = CREATE_CVAR("sk_islave_dmg_claw2", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_dmg_claw3 = CREATE_CVAR("sk_islave_dmg_claw3", "0", 0, 0.0f, nullptr);

cvar_t sk_islave_dmg_clawrake1 = CREATE_CVAR("sk_islave_dmg_clawrake1", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_dmg_clawrake2 = CREATE_CVAR("sk_islave_dmg_clawrake2", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_dmg_clawrake3 = CREATE_CVAR("sk_islave_dmg_clawrake3", "0", 0, 0.0f, nullptr);

cvar_t sk_islave_dmg_zap1 = CREATE_CVAR("sk_islave_dmg_zap1", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_dmg_zap2 = CREATE_CVAR("sk_islave_dmg_zap2", "0", 0, 0.0f, nullptr);
cvar_t sk_islave_dmg_zap3 = CREATE_CVAR("sk_islave_dmg_zap3", "0", 0, 0.0f, nullptr);

// Icthyosaur
cvar_t sk_ichthyosaur_health1 = CREATE_CVAR("sk_ichthyosaur_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_ichthyosaur_health2 = CREATE_CVAR("sk_ichthyosaur_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_ichthyosaur_health3 = CREATE_CVAR("sk_ichthyosaur_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_ichthyosaur_shake1 = CREATE_CVAR("sk_ichthyosaur_shake1", "0", 0, 0.0f, nullptr);
cvar_t sk_ichthyosaur_shake2 = CREATE_CVAR("sk_ichthyosaur_shake2", "0", 0, 0.0f, nullptr);
cvar_t sk_ichthyosaur_shake3 = CREATE_CVAR("sk_ichthyosaur_shake3", "0", 0, 0.0f, nullptr);

// Leech
cvar_t sk_leech_health1 = CREATE_CVAR("sk_leech_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_leech_health2 = CREATE_CVAR("sk_leech_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_leech_health3 = CREATE_CVAR("sk_leech_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_leech_dmg_bite1 = CREATE_CVAR("sk_leech_dmg_bite1", "0", 0, 0.0f, nullptr);
cvar_t sk_leech_dmg_bite2 = CREATE_CVAR("sk_leech_dmg_bite2", "0", 0, 0.0f, nullptr);
cvar_t sk_leech_dmg_bite3 = CREATE_CVAR("sk_leech_dmg_bite3", "0", 0, 0.0f, nullptr);

// Controller
cvar_t sk_controller_health1 = CREATE_CVAR("sk_controller_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_health2 = CREATE_CVAR("sk_controller_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_health3 = CREATE_CVAR("sk_controller_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_controller_dmgzap1 = CREATE_CVAR("sk_controller_dmgzap1", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_dmgzap2 = CREATE_CVAR("sk_controller_dmgzap2", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_dmgzap3 = CREATE_CVAR("sk_controller_dmgzap3", "0", 0, 0.0f, nullptr);

cvar_t sk_controller_speedball1 = CREATE_CVAR("sk_controller_speedball1", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_speedball2 = CREATE_CVAR("sk_controller_speedball2", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_speedball3 = CREATE_CVAR("sk_controller_speedball3", "0", 0, 0.0f, nullptr);

cvar_t sk_controller_dmgball1 = CREATE_CVAR("sk_controller_dmgball1", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_dmgball2 = CREATE_CVAR("sk_controller_dmgball2", "0", 0, 0.0f, nullptr);
cvar_t sk_controller_dmgball3 = CREATE_CVAR("sk_controller_dmgball3", "0", 0, 0.0f, nullptr);

// Nihilanth
cvar_t sk_nihilanth_health1 = CREATE_CVAR("sk_nihilanth_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_nihilanth_health2 = CREATE_CVAR("sk_nihilanth_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_nihilanth_health3 = CREATE_CVAR("sk_nihilanth_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_nihilanth_zap1 = CREATE_CVAR("sk_nihilanth_zap1", "0", 0, 0.0f, nullptr);
cvar_t sk_nihilanth_zap2 = CREATE_CVAR("sk_nihilanth_zap2", "0", 0, 0.0f, nullptr);
cvar_t sk_nihilanth_zap3 = CREATE_CVAR("sk_nihilanth_zap3", "0", 0, 0.0f, nullptr);

// Scientist
cvar_t sk_scientist_health1 = CREATE_CVAR("sk_scientist_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_scientist_health2 = CREATE_CVAR("sk_scientist_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_scientist_health3 = CREATE_CVAR("sk_scientist_health3", "0", 0, 0.0f, nullptr);

// Snark
cvar_t sk_snark_health1 = CREATE_CVAR("sk_snark_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_snark_health2 = CREATE_CVAR("sk_snark_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_snark_health3 = CREATE_CVAR("sk_snark_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_snark_dmg_bite1 = CREATE_CVAR("sk_snark_dmg_bite1", "0", 0, 0.0f, nullptr);
cvar_t sk_snark_dmg_bite2 = CREATE_CVAR("sk_snark_dmg_bite2", "0", 0, 0.0f, nullptr);
cvar_t sk_snark_dmg_bite3 = CREATE_CVAR("sk_snark_dmg_bite3", "0", 0, 0.0f, nullptr);

cvar_t sk_snark_dmg_pop1 = CREATE_CVAR("sk_snark_dmg_pop1", "0", 0, 0.0f, nullptr);
cvar_t sk_snark_dmg_pop2 = CREATE_CVAR("sk_snark_dmg_pop2", "0", 0, 0.0f, nullptr);
cvar_t sk_snark_dmg_pop3 = CREATE_CVAR("sk_snark_dmg_pop3", "0", 0, 0.0f, nullptr);

// Zombie
cvar_t sk_zombie_health1 = CREATE_CVAR("sk_zombie_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_zombie_health2 = CREATE_CVAR("sk_zombie_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_zombie_health3 = CREATE_CVAR("sk_zombie_health3", "0", 0, 0.0f, nullptr);

cvar_t sk_zombie_dmg_one_slash1 = CREATE_CVAR("sk_zombie_dmg_one_slash1", "0", 0, 0.0f, nullptr);
cvar_t sk_zombie_dmg_one_slash2 = CREATE_CVAR("sk_zombie_dmg_one_slash2", "0", 0, 0.0f, nullptr);
cvar_t sk_zombie_dmg_one_slash3 = CREATE_CVAR("sk_zombie_dmg_one_slash3", "0", 0, 0.0f, nullptr);

cvar_t sk_zombie_dmg_both_slash1 = CREATE_CVAR("sk_zombie_dmg_both_slash1", "0", 0, 0.0f, nullptr);
cvar_t sk_zombie_dmg_both_slash2 = CREATE_CVAR("sk_zombie_dmg_both_slash2", "0", 0, 0.0f, nullptr);
cvar_t sk_zombie_dmg_both_slash3 = CREATE_CVAR("sk_zombie_dmg_both_slash3", "0", 0, 0.0f, nullptr);

// Turret
cvar_t sk_turret_health1 = CREATE_CVAR("sk_turret_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_turret_health2 = CREATE_CVAR("sk_turret_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_turret_health3 = CREATE_CVAR("sk_turret_health3", "0", 0, 0.0f, nullptr);

// MiniTurret
cvar_t sk_miniturret_health1 = CREATE_CVAR("sk_miniturret_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_miniturret_health2 = CREATE_CVAR("sk_miniturret_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_miniturret_health3 = CREATE_CVAR("sk_miniturret_health3", "0", 0, 0.0f, nullptr);

// Sentry Turret
cvar_t sk_sentry_health1 = CREATE_CVAR("sk_sentry_health1", "0", 0, 0.0f, nullptr);
cvar_t sk_sentry_health2 = CREATE_CVAR("sk_sentry_health2", "0", 0, 0.0f, nullptr);
cvar_t sk_sentry_health3 = CREATE_CVAR("sk_sentry_health3", "0", 0, 0.0f, nullptr);

// PLAYER WEAPONS

// Crowbar whack
cvar_t sk_plr_crowbar1 = CREATE_CVAR("sk_plr_crowbar1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_crowbar2 = CREATE_CVAR("sk_plr_crowbar2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_crowbar3 = CREATE_CVAR("sk_plr_crowbar3", "0", 0, 0.0f, nullptr);

// Glock Round
cvar_t sk_plr_9mm_bullet1 = CREATE_CVAR("sk_plr_9mm_bullet1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_9mm_bullet2 = CREATE_CVAR("sk_plr_9mm_bullet2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_9mm_bullet3 = CREATE_CVAR("sk_plr_9mm_bullet3", "0", 0, 0.0f, nullptr);

// 357 Round
cvar_t sk_plr_357_bullet1 = CREATE_CVAR("sk_plr_357_bullet1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_357_bullet2 = CREATE_CVAR("sk_plr_357_bullet2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_357_bullet3 = CREATE_CVAR("sk_plr_357_bullet3", "0", 0, 0.0f, nullptr);

// MP5 Round
cvar_t sk_plr_9mmAR_bullet1 = CREATE_CVAR("sk_plr_9mmAR_bullet1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_9mmAR_bullet2 = CREATE_CVAR("sk_plr_9mmAR_bullet2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_9mmAR_bullet3 = CREATE_CVAR("sk_plr_9mmAR_bullet3", "0", 0, 0.0f, nullptr);

// M203 grenade
cvar_t sk_plr_9mmAR_grenade1 = CREATE_CVAR("sk_plr_9mmAR_grenade1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_9mmAR_grenade2 = CREATE_CVAR("sk_plr_9mmAR_grenade2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_9mmAR_grenade3 = CREATE_CVAR("sk_plr_9mmAR_grenade3", "0", 0, 0.0f, nullptr);

// Shotgun buckshot
cvar_t sk_plr_buckshot1 = CREATE_CVAR("sk_plr_buckshot1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_buckshot2 = CREATE_CVAR("sk_plr_buckshot2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_buckshot3 = CREATE_CVAR("sk_plr_buckshot3", "0", 0, 0.0f, nullptr);

// Crossbow
cvar_t sk_plr_xbow_bolt_client1 = CREATE_CVAR("sk_plr_xbow_bolt_client1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_xbow_bolt_client2 = CREATE_CVAR("sk_plr_xbow_bolt_client2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_xbow_bolt_client3 = CREATE_CVAR("sk_plr_xbow_bolt_client3", "0", 0, 0.0f, nullptr);

cvar_t sk_plr_xbow_bolt_monster1 = CREATE_CVAR("sk_plr_xbow_bolt_monster1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_xbow_bolt_monster2 = CREATE_CVAR("sk_plr_xbow_bolt_monster2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_xbow_bolt_monster3 = CREATE_CVAR("sk_plr_xbow_bolt_monster3", "0", 0, 0.0f, nullptr);

// RPG
cvar_t sk_plr_rpg1 = CREATE_CVAR("sk_plr_rpg1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_rpg2 = CREATE_CVAR("sk_plr_rpg2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_rpg3 = CREATE_CVAR("sk_plr_rpg3", "0", 0, 0.0f, nullptr);

// Zero Point Generator
cvar_t sk_plr_gauss1 = CREATE_CVAR("sk_plr_gauss1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_gauss2 = CREATE_CVAR("sk_plr_gauss2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_gauss3 = CREATE_CVAR("sk_plr_gauss3", "0", 0, 0.0f, nullptr);

// Tau Cannon
cvar_t sk_plr_egon_narrow1 = CREATE_CVAR("sk_plr_egon_narrow1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_egon_narrow2 = CREATE_CVAR("sk_plr_egon_narrow2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_egon_narrow3 = CREATE_CVAR("sk_plr_egon_narrow3", "0", 0, 0.0f, nullptr);

cvar_t sk_plr_egon_wide1 = CREATE_CVAR("sk_plr_egon_wide1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_egon_wide2 = CREATE_CVAR("sk_plr_egon_wide2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_egon_wide3 = CREATE_CVAR("sk_plr_egon_wide3", "0", 0, 0.0f, nullptr);

// Hand Grendade
cvar_t sk_plr_hand_grenade1 = CREATE_CVAR("sk_plr_hand_grenade1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_hand_grenade2 = CREATE_CVAR("sk_plr_hand_grenade2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_hand_grenade3 = CREATE_CVAR("sk_plr_hand_grenade3", "0", 0, 0.0f, nullptr);

// Satchel Charge
cvar_t sk_plr_satchel1 = CREATE_CVAR("sk_plr_satchel1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_satchel2 = CREATE_CVAR("sk_plr_satchel2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_satchel3 = CREATE_CVAR("sk_plr_satchel3", "0", 0, 0.0f, nullptr);

// Tripmine
cvar_t sk_plr_tripmine1 = CREATE_CVAR("sk_plr_tripmine1", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_tripmine2 = CREATE_CVAR("sk_plr_tripmine2", "0", 0, 0.0f, nullptr);
cvar_t sk_plr_tripmine3 = CREATE_CVAR("sk_plr_tripmine3", "0", 0, 0.0f, nullptr);

// WORLD WEAPONS
cvar_t sk_12mm_bullet1 = CREATE_CVAR("sk_12mm_bullet1", "0", 0, 0.0f, nullptr);
cvar_t sk_12mm_bullet2 = CREATE_CVAR("sk_12mm_bullet2", "0", 0, 0.0f, nullptr);
cvar_t sk_12mm_bullet3 = CREATE_CVAR("sk_12mm_bullet3", "0", 0, 0.0f, nullptr);

cvar_t sk_9mmAR_bullet1 = CREATE_CVAR("sk_9mmAR_bullet1", "0", 0, 0.0f, nullptr);
cvar_t sk_9mmAR_bullet2 = CREATE_CVAR("sk_9mmAR_bullet2", "0", 0, 0.0f, nullptr);
cvar_t sk_9mmAR_bullet3 = CREATE_CVAR("sk_9mmAR_bullet3", "0", 0, 0.0f, nullptr);

cvar_t sk_9mm_bullet1 = CREATE_CVAR("sk_9mm_bullet1", "0", 0, 0.0f, nullptr);
cvar_t sk_9mm_bullet2 = CREATE_CVAR("sk_9mm_bullet2", "0", 0, 0.0f, nullptr);
cvar_t sk_9mm_bullet3 = CREATE_CVAR("sk_9mm_bullet3", "0", 0, 0.0f, nullptr);

// HORNET
cvar_t sk_hornet_dmg1 = CREATE_CVAR("sk_hornet_dmg1", "0", 0, 0.0f, nullptr);
cvar_t sk_hornet_dmg2 = CREATE_CVAR("sk_hornet_dmg2", "0", 0, 0.0f, nullptr);
cvar_t sk_hornet_dmg3 = CREATE_CVAR("sk_hornet_dmg3", "0", 0, 0.0f, nullptr);

// HEALTH/CHARGE
cvar_t sk_suitcharger1 = CREATE_CVAR("sk_suitcharger1", "0", 0, 0.0f, nullptr);
cvar_t sk_suitcharger2 = CREATE_CVAR("sk_suitcharger2", "0", 0, 0.0f, nullptr);
cvar_t sk_suitcharger3 = CREATE_CVAR("sk_suitcharger3", "0", 0, 0.0f, nullptr);

cvar_t sk_battery1 = CREATE_CVAR("sk_battery1", "0", 0, 0.0f, nullptr);
cvar_t sk_battery2 = CREATE_CVAR("sk_battery2", "0", 0, 0.0f, nullptr);
cvar_t sk_battery3 = CREATE_CVAR("sk_battery3", "0", 0, 0.0f, nullptr);

cvar_t sk_healthcharger1 = CREATE_CVAR("sk_healthcharger1", "0", 0, 0.0f, nullptr);
cvar_t sk_healthcharger2 = CREATE_CVAR("sk_healthcharger2", "0", 0, 0.0f, nullptr);
cvar_t sk_healthcharger3 = CREATE_CVAR("sk_healthcharger3", "0", 0, 0.0f, nullptr);

cvar_t sk_healthkit1 = CREATE_CVAR("sk_healthkit1", "0", 0, 0.0f, nullptr);
cvar_t sk_healthkit2 = CREATE_CVAR("sk_healthkit2", "0", 0, 0.0f, nullptr);
cvar_t sk_healthkit3 = CREATE_CVAR("sk_healthkit3", "0", 0, 0.0f, nullptr);

cvar_t sk_scientist_heal1 = CREATE_CVAR("sk_scientist_heal1", "0", 0, 0.0f, nullptr);
cvar_t sk_scientist_heal2 = CREATE_CVAR("sk_scientist_heal2", "0", 0, 0.0f, nullptr);
cvar_t sk_scientist_heal3 = CREATE_CVAR("sk_scientist_heal3", "0", 0, 0.0f, nullptr);

// monster damage adjusters
cvar_t sk_monster_head1 = CREATE_CVAR("sk_monster_head1", "2", 0, 2.0f, nullptr);
cvar_t sk_monster_head2 = CREATE_CVAR("sk_monster_head2", "2", 0, 2.0f, nullptr);
cvar_t sk_monster_head3 = CREATE_CVAR("sk_monster_head3", "2", 0, 2.0f, nullptr);

cvar_t sk_monster_chest1 = CREATE_CVAR("sk_monster_chest1", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_chest2 = CREATE_CVAR("sk_monster_chest2", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_chest3 = CREATE_CVAR("sk_monster_chest3", "1", 0, 1.0f, nullptr);

cvar_t sk_monster_stomach1 = CREATE_CVAR("sk_monster_stomach1", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_stomach2 = CREATE_CVAR("sk_monster_stomach2", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_stomach3 = CREATE_CVAR("sk_monster_stomach3", "1", 0, 1.0f, nullptr);

cvar_t sk_monster_arm1 = CREATE_CVAR("sk_monster_arm1", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_arm2 = CREATE_CVAR("sk_monster_arm2", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_arm3 = CREATE_CVAR("sk_monster_arm3", "1", 0, 1.0f, nullptr);

cvar_t sk_monster_leg1 = CREATE_CVAR("sk_monster_leg1", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_leg2 = CREATE_CVAR("sk_monster_leg2", "1", 0, 1.0f, nullptr);
cvar_t sk_monster_leg3 = CREATE_CVAR("sk_monster_leg3", "1", 0, 1.0f, nullptr);

// player damage adjusters
cvar_t sk_player_head1 = CREATE_CVAR("sk_player_head1", "2", 0, 2.0f, nullptr);
cvar_t sk_player_head2 = CREATE_CVAR("sk_player_head2", "2", 0, 2.0f, nullptr);
cvar_t sk_player_head3 = CREATE_CVAR("sk_player_head3", "2", 0, 2.0f, nullptr);

cvar_t sk_player_chest1 = CREATE_CVAR("sk_player_chest1", "1", 0, 1.0f, nullptr);
cvar_t sk_player_chest2 = CREATE_CVAR("sk_player_chest2", "1", 0, 1.0f, nullptr);
cvar_t sk_player_chest3 = CREATE_CVAR("sk_player_chest3", "1", 0, 1.0f, nullptr);

cvar_t sk_player_stomach1 = CREATE_CVAR("sk_player_stomach1", "1", 0, 1.0f, nullptr);
cvar_t sk_player_stomach2 = CREATE_CVAR("sk_player_stomach2", "1", 0, 1.0f, nullptr);
cvar_t sk_player_stomach3 = CREATE_CVAR("sk_player_stomach3", "1", 0, 1.0f, nullptr);

cvar_t sk_player_arm1 = CREATE_CVAR("sk_player_arm1", "1", 0, 1.0f, nullptr);
cvar_t sk_player_arm2 = CREATE_CVAR("sk_player_arm2", "1", 0, 1.0f, nullptr);
cvar_t sk_player_arm3 = CREATE_CVAR("sk_player_arm3", "1", 0, 1.0f, nullptr);

cvar_t sk_player_leg1 = CREATE_CVAR("sk_player_leg1", "1", 0, 1.0f, nullptr);
cvar_t sk_player_leg2 = CREATE_CVAR("sk_player_leg2", "1", 0, 1.0f, nullptr);
cvar_t sk_player_leg3 = CREATE_CVAR("sk_player_leg3", "1", 0, 1.0f, nullptr);

// END Cvars for Skill Level settings

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit(void)
{
	IProjectInterface::SetProjectInterfaceImpl(ProjectInterface_Server::StaticInstance());
	CGameResources::StaticInstance().Initialise();

	// Register cvars here:

	g_psv_gravity = CVAR_GET_POINTER("sv_gravity");
	g_psv_aim = CVAR_GET_POINTER("sv_aim");
	g_footsteps = CVAR_GET_POINTER("mp_footsteps");

	CVAR_REGISTER(&displaysoundlist);
	CVAR_REGISTER(&allow_spectators);

	CVAR_REGISTER(&teamplay);
	CVAR_REGISTER(&fraglimit);
	CVAR_REGISTER(&timelimit);

	CVAR_REGISTER(&fragsleft);
	CVAR_REGISTER(&timeleft);

	CVAR_REGISTER(&friendlyfire);
	CVAR_REGISTER(&falldamage);
	CVAR_REGISTER(&weaponstay);
	CVAR_REGISTER(&selfgauss);
	CVAR_REGISTER(&satchelfix);
	CVAR_REGISTER(&forcerespawn);
	CVAR_REGISTER(&flashlight);
	CVAR_REGISTER(&aimcrosshair);
	CVAR_REGISTER(&decalfrequency);
	CVAR_REGISTER(&teamlist);
	CVAR_REGISTER(&teamoverride);
	CVAR_REGISTER(&defaultteam);
	CVAR_REGISTER(&allowmonsters);
	CVAR_REGISTER(&bhopcap);
	CVAR_REGISTER(&multibyte_only);

	CVAR_REGISTER(&mp_chattime);

	// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Agrunt
	CVAR_REGISTER(&sk_agrunt_health1);  // {"sk_agrunt_health1","0");
	CVAR_REGISTER(&sk_agrunt_health2);  // {"sk_agrunt_health2","0");
	CVAR_REGISTER(&sk_agrunt_health3);  // {"sk_agrunt_health3","0");

	CVAR_REGISTER(&sk_agrunt_dmg_punch1);  // {"sk_agrunt_dmg_punch1","0");
	CVAR_REGISTER(&sk_agrunt_dmg_punch2);  // {"sk_agrunt_dmg_punch2","0");
	CVAR_REGISTER(&sk_agrunt_dmg_punch3);  // {"sk_agrunt_dmg_punch3","0");

	// Apache
	CVAR_REGISTER(&sk_apache_health1);  // {"sk_apache_health1","0");
	CVAR_REGISTER(&sk_apache_health2);  // {"sk_apache_health2","0");
	CVAR_REGISTER(&sk_apache_health3);  // {"sk_apache_health3","0");

	// Barney
	CVAR_REGISTER(&sk_barney_health1);  // {"sk_barney_health1","0");
	CVAR_REGISTER(&sk_barney_health2);  // {"sk_barney_health2","0");
	CVAR_REGISTER(&sk_barney_health3);  // {"sk_barney_health3","0");

	// Bullsquid
	CVAR_REGISTER(&sk_bullsquid_health1);  // {"sk_bullsquid_health1","0");
	CVAR_REGISTER(&sk_bullsquid_health2);  // {"sk_bullsquid_health2","0");
	CVAR_REGISTER(&sk_bullsquid_health3);  // {"sk_bullsquid_health3","0");

	CVAR_REGISTER(&sk_bullsquid_dmg_bite1);  // {"sk_bullsquid_dmg_bite1","0");
	CVAR_REGISTER(&sk_bullsquid_dmg_bite2);  // {"sk_bullsquid_dmg_bite2","0");
	CVAR_REGISTER(&sk_bullsquid_dmg_bite3);  // {"sk_bullsquid_dmg_bite3","0");

	CVAR_REGISTER(&sk_bullsquid_dmg_whip1);  // {"sk_bullsquid_dmg_whip1","0");
	CVAR_REGISTER(&sk_bullsquid_dmg_whip2);  // {"sk_bullsquid_dmg_whip2","0");
	CVAR_REGISTER(&sk_bullsquid_dmg_whip3);  // {"sk_bullsquid_dmg_whip3","0");

	CVAR_REGISTER(&sk_bullsquid_dmg_spit1);  // {"sk_bullsquid_dmg_spit1","0");
	CVAR_REGISTER(&sk_bullsquid_dmg_spit2);  // {"sk_bullsquid_dmg_spit2","0");
	CVAR_REGISTER(&sk_bullsquid_dmg_spit3);  // {"sk_bullsquid_dmg_spit3","0");

	CVAR_REGISTER(&sk_bigmomma_health_factor1);  // {"sk_bigmomma_health_factor1","1.0");
	CVAR_REGISTER(&sk_bigmomma_health_factor2);  // {"sk_bigmomma_health_factor2","1.0");
	CVAR_REGISTER(&sk_bigmomma_health_factor3);  // {"sk_bigmomma_health_factor3","1.0");

	CVAR_REGISTER(&sk_bigmomma_dmg_slash1);  // {"sk_bigmomma_dmg_slash1","50");
	CVAR_REGISTER(&sk_bigmomma_dmg_slash2);  // {"sk_bigmomma_dmg_slash2","50");
	CVAR_REGISTER(&sk_bigmomma_dmg_slash3);  // {"sk_bigmomma_dmg_slash3","50");

	CVAR_REGISTER(&sk_bigmomma_dmg_blast1);  // {"sk_bigmomma_dmg_blast1","100");
	CVAR_REGISTER(&sk_bigmomma_dmg_blast2);  // {"sk_bigmomma_dmg_blast2","100");
	CVAR_REGISTER(&sk_bigmomma_dmg_blast3);  // {"sk_bigmomma_dmg_blast3","100");

	CVAR_REGISTER(&sk_bigmomma_radius_blast1);  // {"sk_bigmomma_radius_blast1","250");
	CVAR_REGISTER(&sk_bigmomma_radius_blast2);  // {"sk_bigmomma_radius_blast2","250");
	CVAR_REGISTER(&sk_bigmomma_radius_blast3);  // {"sk_bigmomma_radius_blast3","250");

	// Gargantua
	CVAR_REGISTER(&sk_gargantua_health1);  // {"sk_gargantua_health1","0");
	CVAR_REGISTER(&sk_gargantua_health2);  // {"sk_gargantua_health2","0");
	CVAR_REGISTER(&sk_gargantua_health3);  // {"sk_gargantua_health3","0");

	CVAR_REGISTER(&sk_gargantua_dmg_slash1);  // {"sk_gargantua_dmg_slash1","0");
	CVAR_REGISTER(&sk_gargantua_dmg_slash2);  // {"sk_gargantua_dmg_slash2","0");
	CVAR_REGISTER(&sk_gargantua_dmg_slash3);  // {"sk_gargantua_dmg_slash3","0");

	CVAR_REGISTER(&sk_gargantua_dmg_fire1);  // {"sk_gargantua_dmg_fire1","0");
	CVAR_REGISTER(&sk_gargantua_dmg_fire2);  // {"sk_gargantua_dmg_fire2","0");
	CVAR_REGISTER(&sk_gargantua_dmg_fire3);  // {"sk_gargantua_dmg_fire3","0");

	CVAR_REGISTER(&sk_gargantua_dmg_stomp1);  // {"sk_gargantua_dmg_stomp1","0");
	CVAR_REGISTER(&sk_gargantua_dmg_stomp2);  // {"sk_gargantua_dmg_stomp2","0");
	CVAR_REGISTER(&sk_gargantua_dmg_stomp3);  // {"sk_gargantua_dmg_stomp3","0");

	// Hassassin
	CVAR_REGISTER(&sk_hassassin_health1);  // {"sk_hassassin_health1","0");
	CVAR_REGISTER(&sk_hassassin_health2);  // {"sk_hassassin_health2","0");
	CVAR_REGISTER(&sk_hassassin_health3);  // {"sk_hassassin_health3","0");

	// Headcrab
	CVAR_REGISTER(&sk_headcrab_health1);  // {"sk_headcrab_health1","0");
	CVAR_REGISTER(&sk_headcrab_health2);  // {"sk_headcrab_health2","0");
	CVAR_REGISTER(&sk_headcrab_health3);  // {"sk_headcrab_health3","0");

	CVAR_REGISTER(&sk_headcrab_dmg_bite1);  // {"sk_headcrab_dmg_bite1","0");
	CVAR_REGISTER(&sk_headcrab_dmg_bite2);  // {"sk_headcrab_dmg_bite2","0");
	CVAR_REGISTER(&sk_headcrab_dmg_bite3);  // {"sk_headcrab_dmg_bite3","0");

	// Hgrunt
	CVAR_REGISTER(&sk_hgrunt_health1);  // {"sk_hgrunt_health1","0");
	CVAR_REGISTER(&sk_hgrunt_health2);  // {"sk_hgrunt_health2","0");
	CVAR_REGISTER(&sk_hgrunt_health3);  // {"sk_hgrunt_health3","0");

	CVAR_REGISTER(&sk_hgrunt_kick1);  // {"sk_hgrunt_kick1","0");
	CVAR_REGISTER(&sk_hgrunt_kick2);  // {"sk_hgrunt_kick2","0");
	CVAR_REGISTER(&sk_hgrunt_kick3);  // {"sk_hgrunt_kick3","0");

	CVAR_REGISTER(&sk_hgrunt_pellets1);
	CVAR_REGISTER(&sk_hgrunt_pellets2);
	CVAR_REGISTER(&sk_hgrunt_pellets3);

	CVAR_REGISTER(&sk_hgrunt_gspeed1);
	CVAR_REGISTER(&sk_hgrunt_gspeed2);
	CVAR_REGISTER(&sk_hgrunt_gspeed3);

	// Houndeye
	CVAR_REGISTER(&sk_houndeye_health1);  // {"sk_houndeye_health1","0");
	CVAR_REGISTER(&sk_houndeye_health2);  // {"sk_houndeye_health2","0");
	CVAR_REGISTER(&sk_houndeye_health3);  // {"sk_houndeye_health3","0");

	CVAR_REGISTER(&sk_houndeye_dmg_blast1);  // {"sk_houndeye_dmg_blast1","0");
	CVAR_REGISTER(&sk_houndeye_dmg_blast2);  // {"sk_houndeye_dmg_blast2","0");
	CVAR_REGISTER(&sk_houndeye_dmg_blast3);  // {"sk_houndeye_dmg_blast3","0");

	// ISlave
	CVAR_REGISTER(&sk_islave_health1);  // {"sk_islave_health1","0");
	CVAR_REGISTER(&sk_islave_health2);  // {"sk_islave_health2","0");
	CVAR_REGISTER(&sk_islave_health3);  // {"sk_islave_health3","0");

	CVAR_REGISTER(&sk_islave_dmg_claw1);  // {"sk_islave_dmg_claw1","0");
	CVAR_REGISTER(&sk_islave_dmg_claw2);  // {"sk_islave_dmg_claw2","0");
	CVAR_REGISTER(&sk_islave_dmg_claw3);  // {"sk_islave_dmg_claw3","0");

	CVAR_REGISTER(&sk_islave_dmg_clawrake1);  // {"sk_islave_dmg_clawrake1","0");
	CVAR_REGISTER(&sk_islave_dmg_clawrake2);  // {"sk_islave_dmg_clawrake2","0");
	CVAR_REGISTER(&sk_islave_dmg_clawrake3);  // {"sk_islave_dmg_clawrake3","0");

	CVAR_REGISTER(&sk_islave_dmg_zap1);  // {"sk_islave_dmg_zap1","0");
	CVAR_REGISTER(&sk_islave_dmg_zap2);  // {"sk_islave_dmg_zap2","0");
	CVAR_REGISTER(&sk_islave_dmg_zap3);  // {"sk_islave_dmg_zap3","0");

	// Icthyosaur
	CVAR_REGISTER(&sk_ichthyosaur_health1);  // {"sk_ichthyosaur_health1","0");
	CVAR_REGISTER(&sk_ichthyosaur_health2);  // {"sk_ichthyosaur_health2","0");
	CVAR_REGISTER(&sk_ichthyosaur_health3);  // {"sk_ichthyosaur_health3","0");

	CVAR_REGISTER(&sk_ichthyosaur_shake1);  // {"sk_ichthyosaur_health3","0");
	CVAR_REGISTER(&sk_ichthyosaur_shake2);  // {"sk_ichthyosaur_health3","0");
	CVAR_REGISTER(&sk_ichthyosaur_shake3);  // {"sk_ichthyosaur_health3","0");

	// Leech
	CVAR_REGISTER(&sk_leech_health1);  // {"sk_leech_health1","0");
	CVAR_REGISTER(&sk_leech_health2);  // {"sk_leech_health2","0");
	CVAR_REGISTER(&sk_leech_health3);  // {"sk_leech_health3","0");

	CVAR_REGISTER(&sk_leech_dmg_bite1);  // {"sk_leech_dmg_bite1","0");
	CVAR_REGISTER(&sk_leech_dmg_bite2);  // {"sk_leech_dmg_bite2","0");
	CVAR_REGISTER(&sk_leech_dmg_bite3);  // {"sk_leech_dmg_bite3","0");

	// Controller
	CVAR_REGISTER(&sk_controller_health1);
	CVAR_REGISTER(&sk_controller_health2);
	CVAR_REGISTER(&sk_controller_health3);

	CVAR_REGISTER(&sk_controller_dmgzap1);
	CVAR_REGISTER(&sk_controller_dmgzap2);
	CVAR_REGISTER(&sk_controller_dmgzap3);

	CVAR_REGISTER(&sk_controller_speedball1);
	CVAR_REGISTER(&sk_controller_speedball2);
	CVAR_REGISTER(&sk_controller_speedball3);

	CVAR_REGISTER(&sk_controller_dmgball1);
	CVAR_REGISTER(&sk_controller_dmgball2);
	CVAR_REGISTER(&sk_controller_dmgball3);

	// Nihilanth
	CVAR_REGISTER(&sk_nihilanth_health1);  // {"sk_nihilanth_health1","0");
	CVAR_REGISTER(&sk_nihilanth_health2);  // {"sk_nihilanth_health2","0");
	CVAR_REGISTER(&sk_nihilanth_health3);  // {"sk_nihilanth_health3","0");

	CVAR_REGISTER(&sk_nihilanth_zap1);
	CVAR_REGISTER(&sk_nihilanth_zap2);
	CVAR_REGISTER(&sk_nihilanth_zap3);

	// Scientist
	CVAR_REGISTER(&sk_scientist_health1);  // {"sk_scientist_health1","0");
	CVAR_REGISTER(&sk_scientist_health2);  // {"sk_scientist_health2","0");
	CVAR_REGISTER(&sk_scientist_health3);  // {"sk_scientist_health3","0");

	// Snark
	CVAR_REGISTER(&sk_snark_health1);  // {"sk_snark_health1","0");
	CVAR_REGISTER(&sk_snark_health2);  // {"sk_snark_health2","0");
	CVAR_REGISTER(&sk_snark_health3);  // {"sk_snark_health3","0");

	CVAR_REGISTER(&sk_snark_dmg_bite1);  // {"sk_snark_dmg_bite1","0");
	CVAR_REGISTER(&sk_snark_dmg_bite2);  // {"sk_snark_dmg_bite2","0");
	CVAR_REGISTER(&sk_snark_dmg_bite3);  // {"sk_snark_dmg_bite3","0");

	CVAR_REGISTER(&sk_snark_dmg_pop1);  // {"sk_snark_dmg_pop1","0");
	CVAR_REGISTER(&sk_snark_dmg_pop2);  // {"sk_snark_dmg_pop2","0");
	CVAR_REGISTER(&sk_snark_dmg_pop3);  // {"sk_snark_dmg_pop3","0");

	// Zombie
	CVAR_REGISTER(&sk_zombie_health1);  // {"sk_zombie_health1","0");
	CVAR_REGISTER(&sk_zombie_health2);  // {"sk_zombie_health3","0");
	CVAR_REGISTER(&sk_zombie_health3);  // {"sk_zombie_health3","0");

	CVAR_REGISTER(&sk_zombie_dmg_one_slash1);  // {"sk_zombie_dmg_one_slash1","0");
	CVAR_REGISTER(&sk_zombie_dmg_one_slash2);  // {"sk_zombie_dmg_one_slash2","0");
	CVAR_REGISTER(&sk_zombie_dmg_one_slash3);  // {"sk_zombie_dmg_one_slash3","0");

	CVAR_REGISTER(&sk_zombie_dmg_both_slash1);  // {"sk_zombie_dmg_both_slash1","0");
	CVAR_REGISTER(&sk_zombie_dmg_both_slash2);  // {"sk_zombie_dmg_both_slash2","0");
	CVAR_REGISTER(&sk_zombie_dmg_both_slash3);  // {"sk_zombie_dmg_both_slash3","0");

	// Turret
	CVAR_REGISTER(&sk_turret_health1);  // {"sk_turret_health1","0");
	CVAR_REGISTER(&sk_turret_health2);  // {"sk_turret_health2","0");
	CVAR_REGISTER(&sk_turret_health3);  // {"sk_turret_health3","0");

	// MiniTurret
	CVAR_REGISTER(&sk_miniturret_health1);  // {"sk_miniturret_health1","0");
	CVAR_REGISTER(&sk_miniturret_health2);  // {"sk_miniturret_health2","0");
	CVAR_REGISTER(&sk_miniturret_health3);  // {"sk_miniturret_health3","0");

	// Sentry Turret
	CVAR_REGISTER(&sk_sentry_health1);  // {"sk_sentry_health1","0");
	CVAR_REGISTER(&sk_sentry_health2);  // {"sk_sentry_health2","0");
	CVAR_REGISTER(&sk_sentry_health3);  // {"sk_sentry_health3","0");

	// PLAYER WEAPONS

	// Crowbar whack
	CVAR_REGISTER(&sk_plr_crowbar1);  // {"sk_plr_crowbar1","0");
	CVAR_REGISTER(&sk_plr_crowbar2);  // {"sk_plr_crowbar2","0");
	CVAR_REGISTER(&sk_plr_crowbar3);  // {"sk_plr_crowbar3","0");

	// Glock Round
	CVAR_REGISTER(&sk_plr_9mm_bullet1);  // {"sk_plr_9mm_bullet1","0");
	CVAR_REGISTER(&sk_plr_9mm_bullet2);  // {"sk_plr_9mm_bullet2","0");
	CVAR_REGISTER(&sk_plr_9mm_bullet3);  // {"sk_plr_9mm_bullet3","0");

	// 357 Round
	CVAR_REGISTER(&sk_plr_357_bullet1);  // {"sk_plr_357_bullet1","0");
	CVAR_REGISTER(&sk_plr_357_bullet2);  // {"sk_plr_357_bullet2","0");
	CVAR_REGISTER(&sk_plr_357_bullet3);  // {"sk_plr_357_bullet3","0");

	// MP5 Round
	CVAR_REGISTER(&sk_plr_9mmAR_bullet1);  // {"sk_plr_9mmAR_bullet1","0");
	CVAR_REGISTER(&sk_plr_9mmAR_bullet2);  // {"sk_plr_9mmAR_bullet2","0");
	CVAR_REGISTER(&sk_plr_9mmAR_bullet3);  // {"sk_plr_9mmAR_bullet3","0");

	// M203 grenade
	CVAR_REGISTER(&sk_plr_9mmAR_grenade1);  // {"sk_plr_9mmAR_grenade1","0");
	CVAR_REGISTER(&sk_plr_9mmAR_grenade2);  // {"sk_plr_9mmAR_grenade2","0");
	CVAR_REGISTER(&sk_plr_9mmAR_grenade3);  // {"sk_plr_9mmAR_grenade3","0");

	// Shotgun buckshot
	CVAR_REGISTER(&sk_plr_buckshot1);  // {"sk_plr_buckshot1","0");
	CVAR_REGISTER(&sk_plr_buckshot2);  // {"sk_plr_buckshot2","0");
	CVAR_REGISTER(&sk_plr_buckshot3);  // {"sk_plr_buckshot3","0");

	// Crossbow
	CVAR_REGISTER(&sk_plr_xbow_bolt_monster1);  // {"sk_plr_xbow_bolt1","0");
	CVAR_REGISTER(&sk_plr_xbow_bolt_monster2);  // {"sk_plr_xbow_bolt2","0");
	CVAR_REGISTER(&sk_plr_xbow_bolt_monster3);  // {"sk_plr_xbow_bolt3","0");

	CVAR_REGISTER(&sk_plr_xbow_bolt_client1);  // {"sk_plr_xbow_bolt1","0");
	CVAR_REGISTER(&sk_plr_xbow_bolt_client2);  // {"sk_plr_xbow_bolt2","0");
	CVAR_REGISTER(&sk_plr_xbow_bolt_client3);  // {"sk_plr_xbow_bolt3","0");

	// RPG
	CVAR_REGISTER(&sk_plr_rpg1);  // {"sk_plr_rpg1","0");
	CVAR_REGISTER(&sk_plr_rpg2);  // {"sk_plr_rpg2","0");
	CVAR_REGISTER(&sk_plr_rpg3);  // {"sk_plr_rpg3","0");

	// Gauss Gun
	CVAR_REGISTER(&sk_plr_gauss1);  // {"sk_plr_gauss1","0");
	CVAR_REGISTER(&sk_plr_gauss2);  // {"sk_plr_gauss2","0");
	CVAR_REGISTER(&sk_plr_gauss3);  // {"sk_plr_gauss3","0");

	// Egon Gun
	CVAR_REGISTER(&sk_plr_egon_narrow1);  // {"sk_plr_egon_narrow1","0");
	CVAR_REGISTER(&sk_plr_egon_narrow2);  // {"sk_plr_egon_narrow2","0");
	CVAR_REGISTER(&sk_plr_egon_narrow3);  // {"sk_plr_egon_narrow3","0");

	CVAR_REGISTER(&sk_plr_egon_wide1);  // {"sk_plr_egon_wide1","0");
	CVAR_REGISTER(&sk_plr_egon_wide2);  // {"sk_plr_egon_wide2","0");
	CVAR_REGISTER(&sk_plr_egon_wide3);  // {"sk_plr_egon_wide3","0");

	// Hand Grendade
	CVAR_REGISTER(&sk_plr_hand_grenade1);  // {"sk_plr_hand_grenade1","0");
	CVAR_REGISTER(&sk_plr_hand_grenade2);  // {"sk_plr_hand_grenade2","0");
	CVAR_REGISTER(&sk_plr_hand_grenade3);  // {"sk_plr_hand_grenade3","0");

	// Satchel Charge
	CVAR_REGISTER(&sk_plr_satchel1);  // {"sk_plr_satchel1","0");
	CVAR_REGISTER(&sk_plr_satchel2);  // {"sk_plr_satchel2","0");
	CVAR_REGISTER(&sk_plr_satchel3);  // {"sk_plr_satchel3","0");

	// Tripmine
	CVAR_REGISTER(&sk_plr_tripmine1);  // {"sk_plr_tripmine1","0");
	CVAR_REGISTER(&sk_plr_tripmine2);  // {"sk_plr_tripmine2","0");
	CVAR_REGISTER(&sk_plr_tripmine3);  // {"sk_plr_tripmine3","0");

	// WORLD WEAPONS
	CVAR_REGISTER(&sk_12mm_bullet1);  // {"sk_12mm_bullet1","0");
	CVAR_REGISTER(&sk_12mm_bullet2);  // {"sk_12mm_bullet2","0");
	CVAR_REGISTER(&sk_12mm_bullet3);  // {"sk_12mm_bullet3","0");

	CVAR_REGISTER(&sk_9mmAR_bullet1);  // {"sk_9mm_bullet1","0");
	CVAR_REGISTER(&sk_9mmAR_bullet2);  // {"sk_9mm_bullet1","0");
	CVAR_REGISTER(&sk_9mmAR_bullet3);  // {"sk_9mm_bullet1","0");

	CVAR_REGISTER(&sk_9mm_bullet1);  // {"sk_9mm_bullet1","0");
	CVAR_REGISTER(&sk_9mm_bullet2);  // {"sk_9mm_bullet2","0");
	CVAR_REGISTER(&sk_9mm_bullet3);  // {"sk_9mm_bullet3","0");

	// HORNET
	CVAR_REGISTER(&sk_hornet_dmg1);  // {"sk_hornet_dmg1","0");
	CVAR_REGISTER(&sk_hornet_dmg2);  // {"sk_hornet_dmg2","0");
	CVAR_REGISTER(&sk_hornet_dmg3);  // {"sk_hornet_dmg3","0");

	// HEALTH/SUIT CHARGE DISTRIBUTION
	CVAR_REGISTER(&sk_suitcharger1);
	CVAR_REGISTER(&sk_suitcharger2);
	CVAR_REGISTER(&sk_suitcharger3);

	CVAR_REGISTER(&sk_battery1);
	CVAR_REGISTER(&sk_battery2);
	CVAR_REGISTER(&sk_battery3);

	CVAR_REGISTER(&sk_healthcharger1);
	CVAR_REGISTER(&sk_healthcharger2);
	CVAR_REGISTER(&sk_healthcharger3);

	CVAR_REGISTER(&sk_healthkit1);
	CVAR_REGISTER(&sk_healthkit2);
	CVAR_REGISTER(&sk_healthkit3);

	CVAR_REGISTER(&sk_scientist_heal1);
	CVAR_REGISTER(&sk_scientist_heal2);
	CVAR_REGISTER(&sk_scientist_heal3);

	// monster damage adjusters
	CVAR_REGISTER(&sk_monster_head1);
	CVAR_REGISTER(&sk_monster_head2);
	CVAR_REGISTER(&sk_monster_head3);

	CVAR_REGISTER(&sk_monster_chest1);
	CVAR_REGISTER(&sk_monster_chest2);
	CVAR_REGISTER(&sk_monster_chest3);

	CVAR_REGISTER(&sk_monster_stomach1);
	CVAR_REGISTER(&sk_monster_stomach2);
	CVAR_REGISTER(&sk_monster_stomach3);

	CVAR_REGISTER(&sk_monster_arm1);
	CVAR_REGISTER(&sk_monster_arm2);
	CVAR_REGISTER(&sk_monster_arm3);

	CVAR_REGISTER(&sk_monster_leg1);
	CVAR_REGISTER(&sk_monster_leg2);
	CVAR_REGISTER(&sk_monster_leg3);

	// player damage adjusters
	CVAR_REGISTER(&sk_player_head1);
	CVAR_REGISTER(&sk_player_head2);
	CVAR_REGISTER(&sk_player_head3);

	CVAR_REGISTER(&sk_player_chest1);
	CVAR_REGISTER(&sk_player_chest2);
	CVAR_REGISTER(&sk_player_chest3);

	CVAR_REGISTER(&sk_player_stomach1);
	CVAR_REGISTER(&sk_player_stomach2);
	CVAR_REGISTER(&sk_player_stomach3);

	CVAR_REGISTER(&sk_player_arm1);
	CVAR_REGISTER(&sk_player_arm2);
	CVAR_REGISTER(&sk_player_arm3);

	CVAR_REGISTER(&sk_player_leg1);
	CVAR_REGISTER(&sk_player_leg2);
	CVAR_REGISTER(&sk_player_leg3);

	CVAR_REGISTER(&mp_respawn_avoid_radius);
	CVAR_REGISTER(&mp_corpse_show_time);

	CWeaponRegistry::StaticInstance().RegisterCvars();

	// END REGISTER CVARS FOR SKILL LEVEL STUFF

	WeaponInaccuracyCvars::Init();
	Bot_RegisterCVars();
	BotCommands::Initialise();
	CustomGeometry::InitialiseCommands();
	HitboxDebugging::Initialise();

	SERVER_COMMAND("exec skill.cfg\n");
}
