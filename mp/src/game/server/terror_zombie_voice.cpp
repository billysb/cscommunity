#include "cbase.h"
#include "cs_shareddefs.h"
#include "cs_player.h"
#include "cs_bot.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CZombieVoice : public CBaseAnimating
{
public:
	DECLARE_CLASS(CZombieVoice, CBaseAnimating);
	DECLARE_DATADESC();


	CZombieVoice()
	{
		m_bIsAngry = false;
		m_fNextMonitorTime = 0;
		m_fNextNoiseTime = 0;
		m_iZombieType = 0;
	}

	void Spawn(void);
	void Precache(void);

	void Start(void);

	void Update(CCSPlayer *pPlayer);
	void Think(void);

	// Input function
	//void InputToggle(inputdata_t &inputData);

private:

	// 0 normal, 1 boomer, 2 smoker.
	int		m_iZombieType;

	// Last noise time.
	float	m_fNextNoiseTime;

	// Update time
	float	m_fNextMonitorTime;
	
	// Angry mode. Make anger sounds.
	bool	m_bIsAngry;

	bool	m_bShouldMakeRageSound;
};

LINK_ENTITY_TO_CLASS(zombiebox, CZombieVoice);

// Start of our data description for the class
BEGIN_DATADESC(CZombieVoice)


// Links our input name from Hammer to our input member function
//DEFINE_INPUTFUNC(FIELD_VOID, "Toggle", InputToggle),

// Declare our think function
DEFINE_THINKFUNC(Think),

END_DATADESC()

void CZombieVoice::Precache(void)
{
	//PrecacheModel("smokergas.mdl");

	PrecacheScriptSound("BaseSmokeEffect.Sound");
	PrecacheScriptSound("Terror.SmokerGas");

	BaseClass::Precache();
}

void CZombieVoice::Spawn(void)
{
	Precache();

	//SetModel(NULL_STRING);
	SetSolid(SOLID_NONE);
	UTIL_SetSize(this, -Vector(100, 100, 20), Vector(100, 100, 100));
}

// Called once gamerules give us an owner.
void CZombieVoice::Start(void)
{
	if (GetOwnerEntity() == NULL)
	{
		// Shit this is no good.
		DevMsg("Zombie voice didn't have a owner entity. This cannot work.\n");
		UTIL_Remove(this);
	}

	// Start thinking
	SetThink(&CZombieVoice::Think);
	SetNextThink(gpGlobals->curtime + 0.05f);
}

void CZombieVoice::Update(CCSPlayer *pPlayer)
{
	if (pPlayer)
	{
		// No longer needed.
		if (pPlayer->IsDead())
			UTIL_Remove(this);


		if (pPlayer->IsBot())
		{
			// A lot of stuff should be handled by the bot except for attacking.
			// TODO: detect if bot is alert.
			CCSBot *me = (CCSBot*)pPlayer;
			
			m_bIsAngry = me->IsAlert();
		}
		
		if (pPlayer->m_nButtons & IN_ATTACK)
		{
			m_bIsAngry = true;
			m_bShouldMakeRageSound = true;
		}
	}
	else
	{
		DevMsg("CZombieVoice cannot update because player is invalid. removing self.\n");
		UTIL_Remove(this);
	}
}

void CZombieVoice::Think(void)
{
	CCSPlayer *body = ToCSPlayer(GetOwnerEntity());

	if (m_fNextMonitorTime < gpGlobals->curtime)
		Update(body);

	if (m_fNextNoiseTime < gpGlobals->curtime)
	{
		// Ready to make a sound.
		if (m_bIsAngry)
		{
			this->EmitSound("Zombie.Idle");
		}
		else
		{
			this->EmitSound("Zombie.AngryIdle");
		}
	}

	SetNextThink(gpGlobals->curtime + 0.5);
}
