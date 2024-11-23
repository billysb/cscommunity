//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "weapon_molotov.h"


#ifdef CLIENT_DLL

#else

#include "cs_player.h"
#include "items.h"
#include "molotov_projectile.h"

#endif


#define GRENADE_TIMER	20.0f //Seconds



IMPLEMENT_NETWORKCLASS_ALIASED(Molotov, DT_Molotov)

BEGIN_NETWORK_TABLE(CMolotov, DT_Molotov)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CMolotov)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_molotov, CMolotov);
PRECACHE_WEAPON_REGISTER(weapon_molotov);


#ifndef CLIENT_DLL

BEGIN_DATADESC(CMolotov)
END_DATADESC()

void CMolotov::EmitGrenade(Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CBasePlayer *pPlayer)
{
	CMolotovProjectile::Create(vecSrc, vecAngles, vecVel, angImpulse, pPlayer, GRENADE_TIMER);
}

#endif

