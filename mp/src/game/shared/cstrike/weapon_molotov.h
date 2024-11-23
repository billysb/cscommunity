//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_MOLOTOV_H
#define WEAPON_MOLOTOV_H
#ifdef _WIN32
#pragma once
#endif


#include "weapon_basecsgrenade.h"


#ifdef CLIENT_DLL

#define CMolotov C_Molotov

#endif

//-----------------------------------------------------------------------------
// Fragmentation grenades
//-----------------------------------------------------------------------------
class CMolotov : public CBaseCSGrenade
{
public:
	DECLARE_CLASS(CMolotov, CBaseCSGrenade);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CMolotov() {}

	virtual CSWeaponID GetWeaponID(void) const		{ return WEAPON_MOLOTOV; }

#ifdef CLIENT_DLL

#else
	DECLARE_DATADESC();

	virtual void EmitGrenade(Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer);

#endif

	CMolotov(const CMolotov &) {}
};


#endif // WEAPON_HEGRENADE_H
