//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "molotov_projectile.h"
#include "soundent.h"
#include "cs_player.h"
#include "KeyValues.h"

#define GRENADE_MODEL "models/Weapons/w_eq_molotov_thrown.mdl"


LINK_ENTITY_TO_CLASS( molotov_projectile, CMolotovProjectile );
PRECACHE_WEAPON_REGISTER( molotov_projectile );

CMolotovProjectile* CMolotovProjectile::Create( 
	const Vector &position, 
	const QAngle &angles, 
	const Vector &velocity, 
	const AngularImpulse &angVelocity, 
	CBaseCombatCharacter *pOwner, 
	float timer )
{
	CMolotovProjectile *pGrenade = (CMolotovProjectile*)CBaseEntity::Create( "molotov_projectile", position, angles, pOwner );
	
	// Set the timer for 1 second less than requested. We're going to issue a SOUND_DANGER
	// one second before detonation.

	pGrenade->SetDetonateTimerLength( 1.5 );
	pGrenade->SetAbsVelocity( velocity );
	pGrenade->SetupInitialTransmittedGrenadeVelocity( velocity );
	pGrenade->SetThrower( pOwner ); 

	pGrenade->SetGravity( BaseClass::GetGrenadeGravity() );
	pGrenade->SetFriction( BaseClass::GetGrenadeFriction() );
	pGrenade->SetElasticity( BaseClass::GetGrenadeElasticity() );

	pGrenade->m_flDamage = 25;
	pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;
	pGrenade->ChangeTeam( pOwner->GetTeamNumber() );
	pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );	

	// make NPCs afaid of it while in the air
	pGrenade->SetThink( &CMolotovProjectile::DangerSoundThink );
	pGrenade->SetNextThink( gpGlobals->curtime );

	return pGrenade;
}

void CMolotovProjectile::Spawn()
{
	SetModel( GRENADE_MODEL );
	BaseClass::Spawn();
}

void CMolotovProjectile::Precache()
{
	PrecacheModel( GRENADE_MODEL );

	BaseClass::Precache();
}

void CMolotovProjectile::BounceSound( void )
{
	// We don't bounce we Detonate
	Detonate();
}

void CMolotovProjectile::Detonate()
{
	BaseClass::Detonate();

	CBaseEntity *fire = CreateEntityByName("ent_firefloor");
	fire->SetAbsOrigin(this->GetAbsOrigin());
	fire->Spawn();

	// tell the bots an HE grenade has exploded
	CCSPlayer *player = ToCSPlayer(GetThrower());
	if ( player )
	{
		IGameEvent * event = gameeventmanager->CreateEvent( "hegrenade_detonate" );
		if ( event )
		{
			event->SetInt( "userid", player->GetUserID() );
			event->SetFloat( "x", GetAbsOrigin().x );
			event->SetFloat( "y", GetAbsOrigin().y );
			event->SetFloat( "z", GetAbsOrigin().z );
			gameeventmanager->FireEvent( event );
		}
	}
}
