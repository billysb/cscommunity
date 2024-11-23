/*
Basic placeholder entity for spawning in fire for the molotov.
TODO: Particle effect is not spawning why?
*/

#include "cbase.h"
#include "particle_smokegrenade.h"
#include "cs_shareddefs.h"
#include "nav_mesh.h"
#include "particle_parse.h"
#include "cs_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAX_MOLOTOV_FLAMES 18

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
	void CFireFloor::CreateFire(Vector pos, int index);
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

void CFireFloor::CreateFire(Vector Pos, int index)
{
	CBaseEntity* pEntity = CreateEntityByName("env_fire");
	if (pEntity)
	{
		variant_t emptyVariant;
		pEntity->SetAbsOrigin(Pos);
		pEntity->Spawn();
		pEntity->Activate();
		pEntity->AcceptInput("StartFire", this, this, emptyVariant, 0);

	}
	createdEntities[index] = pEntity;
}

void CFireFloor::Precache(void)
{
	//PrecacheModel("smokergas.mdl");
	PrecacheParticleSystem("molotov_base");
	PrecacheScriptSound("fire_large");
	PrecacheScriptSound("PropaneTank.Burst");
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

	EmitSound("PropaneTank.Burst");

	SetNextThink(gpGlobals->curtime + 0.05f);

	// Start thinking
	SetThink(&CFireFloor::Think);

}

void CFireFloor::Think(void)
{
	if (!m_bDidFire)
	{
		EmitSound("fire_large");
		
		CreateFire(GetAbsOrigin(), MAX_MOLOTOV_FLAMES);

		int counter = 0;
		while (counter < (MAX_MOLOTOV_FLAMES - 1))
		{
			// Create Vector for direction
			Vector vecOffset(0, 0, 5);

			Vector newpos = GetRandomNearbyPosition(this, 150.0f);

			// Get the Start/End
			Vector vecAbsStart = GetAbsOrigin() + vecOffset;
			Vector vecAbsEnd = newpos;

			trace_t tr;
			
			UTIL_TraceLine(vecAbsStart, vecAbsEnd, MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &tr);

			if (!tr.DidHitWorld())
			{
				// Spawn fire at current location.
				UTIL_AddDebugLine(vecAbsStart, vecAbsEnd, false, false);
			}
			else
			{
				// A second pass allows the fire to have more reach in maps with lots of steps / displacements. 
				// Can be a little weird at times but for the most part works really well.
				UTIL_AddDebugLine(vecAbsStart, vecAbsEnd, false, false);

				// We hit the world. Quickly check if we can trace to the original end position if slightly raised.
				Vector newoffset = Vector(0, 0, 50);
				Vector quickstart = (vecAbsStart - vecAbsEnd) + newoffset;
				Vector quickend = (vecAbsEnd + newoffset);

				trace_t tr2;
				//vecAbsEnd = tr.endpos;

				if (!tr2.DidHitWorld())
				{
					// Fantastic!
					vecAbsEnd = quickend;
					UTIL_TraceLine(quickstart, quickend, MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &tr);
				}
				else
				{
					// Probably still a better position.
					vecAbsEnd = tr2.endpos;
					UTIL_TraceLine(quickstart, vecAbsEnd, MASK_SOLID, this, COLLISION_GROUP_DEBRIS, &tr);
				}
			}

			// Create the flame.
			CreateFire(vecAbsEnd, counter);
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
			//StopLoopingSounds(); // This doesnt do shit?
			StopSound("fire_large");
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

				if (!ppEnts[i]->IsPlayer())
					continue;

				// Valid player or npc. Do damage.
				if (ppEnts[i]->IsAlive())//&& m_iTotalThinks <= 40)
				{
					// TODO: We need a more intelligent way to damage enemies.
					
					Vector Offset = Vector(0, 0, 50);
					Vector vecAbsStart = (GetAbsOrigin() + Offset);
					Vector vecAbsEnd = ppEnts[i]->GetAbsOrigin();

					trace_t tr;

					//ugh hacky because the zombies use this collision group
					UTIL_TraceLine(vecAbsStart, vecAbsEnd, MASK_PLAYERSOLID, this, COLLISION_GROUP_DEBRIS, &tr);

					// Can't see the target. dont hurt because they could be in a room above or below us. / nothing to burn.
					if (!tr.DidHit() || tr.DidHitWorld())
						continue;

					// if we hit an entity that was not the world.
					if (!tr.DidHitNonWorldEntity())
						continue;

					CBaseEntity *target = tr.m_pEnt;

					Msg("Yes!\n");

					if (target != NULL)
					{
						Msg("Yes2!\n");
						CTakeDamageInfo dmginfo;
						dmginfo.SetAttacker(this);
						dmginfo.SetInflictor(this);

						dmginfo.SetDamage(m_fDmgAmount);
						dmginfo.AddDamageType(DMG_BURN);
						dmginfo.SetMaxDamage(10);

						ppEnts[i]->TakeDamage(dmginfo);

						if (CSGameRules()->IsTerrorStrikeMap() && ppEnts[i]->GetTeamNumber() == TEAM_CT)
						{
							// if its terror-strike mode and we are a zombie we burn >:D
							ppEnts[i]->GetBaseAnimating()->Ignite(30.0f, false, 5.0f, false);
						}
					}

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
