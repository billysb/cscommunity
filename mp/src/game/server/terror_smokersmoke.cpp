/*
Navblocking and unblocking doesnt work yet because its fucked.
*/

#include "cbase.h"
#include "particle_smokegrenade.h"
#include "cs_shareddefs.h"
#include "nav_mesh.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CSmokerGas : public CBaseAnimating
{
public:
	DECLARE_CLASS(CSmokerGas, CBaseAnimating);
	DECLARE_DATADESC();


	CSmokerGas()
	{
		m_bDidSmoke = false;
		m_iTotalThinks = 0;
		m_fDmgAmount = 1;
	}

	void Spawn(void);
	void Precache(void);

	void Think(void);

	// Input function
	//void InputToggle(inputdata_t &inputData);

private:

	EHANDLE m_SmokeEffect;
	bool	m_bDidSmoke;
	int		m_iTotalThinks;
	float	m_fDmgAmount;
	CNavArea* m_SpawnArea;
};

LINK_ENTITY_TO_CLASS(ent_smokergas, CSmokerGas);

// Start of our data description for the class
BEGIN_DATADESC(CSmokerGas)


// Links our input name from Hammer to our input member function
//DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),

// Declare our think function
DEFINE_THINKFUNC(Think),

END_DATADESC()

void CSmokerGas::Precache(void)
{
	//PrecacheModel("smokergas.mdl");

	PrecacheScriptSound("BaseSmokeEffect.Sound");
	PrecacheScriptSound("Terror.SmokerGas");

	BaseClass::Precache();
}

void CSmokerGas::Spawn(void)
{
	Precache();

	m_iTotalThinks = 0;
	m_bDidSmoke = false;
	//SetModel(NULL_STRING);
	SetSolid(SOLID_NONE);
	UTIL_SetSize(this, -Vector(100, 100, 20), Vector(100, 100, 100));

	m_SpawnArea = TheNavMesh->GetNavArea(GetAbsOrigin());

	// Start thinking
	SetThink(&CSmokerGas::Think);

	SetNextThink(gpGlobals->curtime + 0.05f);

}

void CSmokerGas::Think(void)
{
	if (!m_bDidSmoke)
	{
		ParticleSmokeGrenade *pGren = (ParticleSmokeGrenade*)CBaseEntity::Create(PARTICLESMOKEGRENADE_ENTITYNAME, GetAbsOrigin(), QAngle(0, 0, 0), NULL);
		if (pGren)
		{
			pGren->FillVolume();
			pGren->SetFadeTime(0, 10);
			pGren->SetAbsOrigin(GetAbsOrigin());

			m_SmokeEffect = pGren;
			m_bDidSmoke = true;
			this->EmitSound("BaseSmokeEffect.Sound");
			//this->EmitSound("Terror.SmokerGas");
			
			// Make terrorist bots avoid.
			if (m_SpawnArea != NULL)
				m_SpawnArea->MarkAsBlocked(TEAM_TERRORIST, this, true);
		}
	}
	else
	{
		if (m_iTotalThinks > 20)
		{
			// Unblock the area.
			if (m_SpawnArea != NULL)
				m_SpawnArea->UnblockArea(TEAM_TERRORIST);

			// Must destroy self.
			if (m_SmokeEffect.Get())
			{
				UTIL_Remove(m_SmokeEffect);
			}
			//Msg("I am fucking dead, deleted from reality.\n");
			Remove();
		}
		else
		{
			//Msg("Count: %d\n", m_iTotalThinks);

			ParticleSmokeGrenade *effect = (ParticleSmokeGrenade*)m_SmokeEffect.Get();
			if (!effect || effect && effect->m_FadeEndTime > gpGlobals->curtime)
			{
				// We finished.
				m_iTotalThinks = 46;
			}


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
				if (ppEnts[i]->IsAlive() && m_iTotalThinks <= 13)
				{
					CTakeDamageInfo dmginfo;
					dmginfo.SetAttacker(this);
					dmginfo.SetInflictor(this);
					
					dmginfo.SetDamage(m_fDmgAmount);
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
