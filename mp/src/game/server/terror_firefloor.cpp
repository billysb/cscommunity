/*
Basic placeholder entity for spawning in fire for the molotov.
*/

#include "cbase.h"
#include "particle_smokegrenade.h"
#include "cs_shareddefs.h"
#include "nav_mesh.h"
#include "particle_parse.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAX_MOLOTOV_FLAMES 14

class CFireFloor : public CBaseAnimating
{
public:
	DECLARE_CLASS(CFireFloor, CBaseAnimating);
	DECLARE_DATADESC();


	CFireFloor()
	{
		m_bDidFire = false;
		m_iTotalThinks = 0;
		m_fDmgAmount = 1;
	}

	void Spawn(void);
	void Precache(void);

	void Think(void);

	// Input function
	//void InputToggle(inputdata_t &inputData);

private:

	bool	m_bDidFire;
	int		m_iTotalThinks;
	float	m_fDmgAmount;
	CNavArea* m_SpawnArea;
	CBaseEntity* createdEntities[MAX_MOLOTOV_FLAMES];
};

LINK_ENTITY_TO_CLASS(ent_firefloor, CFireFloor);

// Start of our data description for the class
BEGIN_DATADESC(CFireFloor)


// Links our input name from Hammer to our input member function
//DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),

// Declare our think function
DEFINE_THINKFUNC(Think),

END_DATADESC()

Vector GetRandomNearbyPosition(CBaseEntity* pEntity, float range) {
	// Get the current position of the entity
	Vector currentPosition = pEntity->GetAbsOrigin();

	// Generate random offsets within the specified range
	float randomX = RandomFloat(-range, range);
	float randomY = RandomFloat(-range, range);
	float randomZ = 0; 

	// Create the new position vector
	Vector randomPosition = currentPosition + Vector(randomX, randomY, randomZ);

	return randomPosition;
}

void CFireFloor::Precache(void)
{
	//PrecacheModel("smokergas.mdl");
	PrecacheParticleSystem("molotov_base");
	PrecacheScriptSound("BaseSmokeEffect.Sound");
	PrecacheScriptSound("Terror.SmokerGas");

	BaseClass::Precache();
}

void CFireFloor::Spawn(void)
{
	Precache();
	
	m_iTotalThinks = 0;
	m_bDidFire = false;
	//SetModel(NULL_STRING);
	SetSolid(SOLID_NONE);
	UTIL_SetSize(this, -Vector(100, 100, 20), Vector(100, 100, 100));

	m_SpawnArea = TheNavMesh->GetNavArea(GetAbsOrigin());
	DispatchParticleEffect("molotov_base", GetAbsOrigin(), GetAbsAngles(), this);
	// Start thinking
	SetThink(&CFireFloor::Think);

	SetNextThink(gpGlobals->curtime + 0.05f);

}

void CFireFloor::Think(void)
{
	if (!m_bDidFire)
	{
		variant_t emptyVariant;
		CBaseEntity* pEntity = CreateEntityByName("env_fire");
		if (pEntity)
		{
			// Set the position of the entity
			pEntity->SetAbsOrigin(GetAbsOrigin());
			pEntity->Spawn();
			pEntity->Activate();
			pEntity->AcceptInput("StartFire", this, this, emptyVariant, 0);

			// Store the entity in the array
			createdEntities[MAX_MOLOTOV_FLAMES] = pEntity;
		}

		int counter = 0;
		while (counter < 10)
		{
			// Create Vector for direction
			Vector vecDir(0, 0, 50);
			Vector vecOffset(0, 0, 5);

			Vector newpos = GetRandomNearbyPosition(this, 150.0f);
			//UTIL_AddDebugLine(newpos, GetAbsOrigin(), false, false);

			// Get the Start/End
			Vector vecAbsStart = GetAbsOrigin() + vecOffset;
			Vector vecAbsEnd = newpos;

			trace_t tr;
			
			UTIL_TraceLine(vecAbsStart, vecAbsEnd, MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &tr);

			if (!tr.DidHitWorld())
			{
				Msg("I did it :D\n");
				// did not hit the world.
				//DispatchParticleEffect("fire_medium_02_nosmoke", vecAbsEnd, GetAbsAngles(), this);
				UTIL_AddDebugLine(vecAbsStart, vecAbsEnd, false, false);
				CBaseEntity* pEntity = CreateEntityByName("env_fire");
				if (pEntity)
				{
					// Set the position of the entity
					pEntity->SetAbsOrigin(vecAbsEnd);
					pEntity->Spawn();
					pEntity->Activate();
					pEntity->AcceptInput("StartFire", this, this, emptyVariant, 0);

					// Store the entity in the array
					createdEntities[counter] = pEntity;
				}
			}
			else
			{
				Vector hitpos = tr.endpos;
				UTIL_AddDebugLine(vecAbsStart, hitpos, false, false);
				//DispatchParticleEffect("fire_medium_02_nosmoke", hitpos, GetAbsAngles(), this);
				CBaseEntity* pEntity = CreateEntityByName("env_fire");
				if (pEntity)
				{
					// Set the position of the entity
					pEntity->SetAbsOrigin(hitpos);
					pEntity->Spawn();
					pEntity->Activate();
					pEntity->AcceptInput("StartFire", this, this, emptyVariant, 0);

					// Store the entity in the array
					createdEntities[counter] = pEntity;
				}
			}
			++counter;
		}
		m_bDidFire = true;
	}
	else
	{
		if (m_iTotalThinks > 50)
		{
			// Unblock the area.
			if (m_SpawnArea != NULL)
				m_SpawnArea->UnblockArea(TEAM_TERRORIST);

			for (int i = 0; i < (MAX_MOLOTOV_FLAMES+1); ++i)
			{
				if (createdEntities[i])
				{
					createdEntities[i]->Remove();
				}
			}

			// Must destroy self.
			StopParticleEffects(this);
			Msg("I am fucking dead, deleted from reality.\n");
			Remove();
		}
		else
		{
			Msg("Count: %d\n", m_iTotalThinks);

			// Do damage.
			Vector vecCenter = WorldSpaceCenter();

			CBaseEntity *ppEnts[256];
			float flRadius = 100.0f;
			//vecCenter.z -= flRadius * 0.8f;
			int nEntCount = UTIL_EntitiesInSphere(ppEnts, 256, vecCenter, flRadius, 0);
			int i;

			for (i = 0; i < nEntCount; i++)
			{
				//Look through the entities it found
				if (ppEnts[i] == NULL)
					continue;

				// Zap metal or flesh things.
				if (!ppEnts[i]->IsPlayer())
					continue;

				// Valid player. Do damage.
				if (ppEnts[i]->IsAlive())//&& m_iTotalThinks <= 40)
				{
					// TODO: We need a more intelligent way to damage enemies.
					
					CTakeDamageInfo dmginfo;
					dmginfo.SetAttacker(this);
					dmginfo.SetInflictor(this);

					dmginfo.SetDamage(m_fDmgAmount);
					dmginfo.AddDamageType(DMG_BURN);
					dmginfo.SetMaxDamage(10);

					ppEnts[i]->TakeDamage(dmginfo);

					//delete &dmginfo;
				}
			}

			// Damage increase over time.
			m_fDmgAmount++;
			m_iTotalThinks++;

			// SetMaxDamage doesnt do shit so we have to do this stupid shit.
			if (m_fDmgAmount > 10)
				m_fDmgAmount = 1;
		}
	}
	SetNextThink(gpGlobals->curtime + 0.5);
}
