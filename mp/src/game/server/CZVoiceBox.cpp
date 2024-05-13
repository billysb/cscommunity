/*
This class basically watches the zombie and decides on what noises to make.
Why?

Because if we hardcode it into the bot code shit gets messy and this lets players on the zombie team feel special too.
*/

#include "cbase.h"
#include "CZVoiceBox.h"
#include "cs_player.h"
#include "cs_bot.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define VOICEBOX_ATTACK_TIME 1.8
#define VOICEBOX_UPDATE_TIME 5
#define VOICEBOX_NOISE_TIME 6

// How long before we become unangry.
#define VOICEBOX_ANGER_TIME 30

#define VOICEBOX_TYPE_NORMAL 1
#define VOICEBOX_TYPE_BOOMER 2
#define VOICEBOX_TYPE_SMOKER 3

// This one is for later.
#define VOICEBOX_TYPE_TANK 4

CZVoiceBox::CZVoiceBox(CCSPlayer* me)
{
	m_bIsAngry = false;
	m_bIsSpecial = false;
	m_bIsSneaking = false;


	m_fUpdateOffset = RandomFloat(0.1, 12.5);
	m_fNoiseOffset = RandomFloat(0.1, 12.5);

	// The random floats make it so the bots HOPEFULLY, update / make noises at different times when joining all at the same time.
	m_fNextAngerTime = gpGlobals->curtime + VOICEBOX_ANGER_TIME;
	m_fNextNoiseTime = (gpGlobals->curtime + VOICEBOX_NOISE_TIME) + m_fUpdateOffset;
	m_fNextUpdateTime = (gpGlobals->curtime + VOICEBOX_UPDATE_TIME) + m_fNoiseOffset;
	m_fLastAttackTime = 0;

	m_hOwner = me;

	// Hacky precache.
	me->PrecacheScriptSound("Zombie.Idle");
	me->PrecacheScriptSound("Zombie.Attack");
	me->PrecacheScriptSound("Zombie.AngryIdle");
	me->PrecacheScriptSound("Zombie.Pain");

	// Specials
	me->PrecacheScriptSound("Smoker.Idle");
	me->PrecacheScriptSound("Smoker.Attack");
	me->PrecacheScriptSound("Smoker.AngryIdle");
	me->PrecacheScriptSound("Smoker.Pain");

	me->PrecacheScriptSound("Boomer.Idle");
	me->PrecacheScriptSound("Boomer.Attack");
	me->PrecacheScriptSound("Boomer.AngryIdle");
	me->PrecacheScriptSound("Boomer.Pain");
}

void CZVoiceBox::SpawnNoise(void)
{
	// If we are spawning then dont be angry.
	m_bIsAngry = false;

	CCSPlayer* body = m_hOwner;

	switch (m_iSpecialType)
	{
	case VOICEBOX_TYPE_SMOKER:
		body->EmitSound("Smoker.Spawn");
		break;
	case VOICEBOX_TYPE_BOOMER:
		body->EmitSound("Boomer.Spawn");
		break;
	default:
		body->EmitSound("Zombie.Alert");
	}
}

void CZVoiceBox::DeathNoise(void)
{
	// Can't be angry anymore I'm dead.
	m_bIsAngry = false;

	CCSPlayer* body = m_hOwner;

	switch (m_iSpecialType)
	{
	case VOICEBOX_TYPE_SMOKER:
		body->EmitSound("Smoker.Death");
		break;
	case VOICEBOX_TYPE_BOOMER:
		body->EmitSound("Boomer.Death");
		break;
	default:
		body->EmitSound("Zombie.Death");
	}
}

void CZVoiceBox::AttackNoise(void)
{
	// last attack time + offset.
	if (m_fLastAttackTime > gpGlobals->curtime)
		return;

	CCSPlayer* body = m_hOwner;

	// It's possible Update will override this but unlikely with constant attacks.
	// Attacking makes us angry.
	m_bIsAngry = true;

	switch (m_iSpecialType)
	{
		case VOICEBOX_TYPE_SMOKER:
			body->EmitSound("Smoker.Attack");
			break;
		case VOICEBOX_TYPE_BOOMER:
			body->EmitSound("Boomer.Attack");
			break;
		default:
			body->EmitSound("Zombie.Attack");
	}

	// Don't spam the attack sound idiot.
	m_fLastAttackTime = gpGlobals->curtime + VOICEBOX_ATTACK_TIME;
	m_fNextAngerTime = gpGlobals->curtime + VOICEBOX_ANGER_TIME;
}

void CZVoiceBox::PainNoise(void)
{
	// Pain makes me angry >:(
	m_bIsAngry = true;

	CCSPlayer* body = m_hOwner;

	// TODO: implement fire pain sounds early.
	switch (m_iSpecialType)
	{
		case VOICEBOX_TYPE_SMOKER:
			body->EmitSound("Smoker.Pain");
			break;
		case VOICEBOX_TYPE_BOOMER:
			body->EmitSound("Boomer.Pain");
			break;
		default:
			body->EmitSound("Zombie.Pain");
	}
}

void CZVoiceBox::ForceUpdate(void)
{
	m_fNextUpdateTime = 0;
	Update();
}

void CZVoiceBox::Update(void)
{
	// Insanely stupid macro to fix compile error of something we will never use outside of terror.
#ifdef TERROR
	if (m_fNextUpdateTime > gpGlobals->curtime)
		return;

	CCSPlayer* body = m_hOwner;


	// No point in updating when we are dead.
	if (!body->IsAlive())
	{
		m_fNextUpdateTime = gpGlobals->curtime + VOICEBOX_UPDATE_TIME;
		return;
	}

	// Check if we shouldnt be angry anymore.
	if (m_fNextAngerTime < gpGlobals->curtime)
	{
		m_bIsAngry = false;
		m_fNextAngerTime = gpGlobals->curtime + VOICEBOX_ANGER_TIME;
	}

	// We are being sneaky to surprise a human.
	if (body->m_nButtons & IN_WALK || body->m_nButtons & IN_DUCK)
		m_bIsSneaking = true;

	// Figure out a voice type.
	m_bIsSpecial = body->m_IsSpecialInfected;

	if (m_bIsSpecial)
	{
		if (body->m_IsSmoker)
			m_iSpecialType = VOICEBOX_TYPE_SMOKER;
		else
			m_iSpecialType = VOICEBOX_TYPE_BOOMER;
	}
	else
	{
		m_iSpecialType = VOICEBOX_TYPE_NORMAL;
	}

	// Are we a bot?
	if (body->IsBot())
	{
		// If we are then we can control our anger ourself.
		CCSBot* bot = (CCSBot*)body;
		m_bIsAngry = bot->IsAttacking();
	}

	// Don't wastefully update.
	m_fNextUpdateTime = gpGlobals->curtime + VOICEBOX_UPDATE_TIME + m_fUpdateOffset;
#endif
}

void CZVoiceBox::Think(void)
{
	CCSPlayer* body = m_hOwner;

	// No point in making sound if we are dead.
	if (!body->IsAlive())
	{
		m_fNextNoiseTime = gpGlobals->curtime + VOICEBOX_NOISE_TIME;
		return;
	}

	// Don't make idle sounds while attacking.
	if (body->m_nButtons & IN_ATTACK)
	{
		// Just wait again its not a big deal.
		AttackNoise();
		m_fNextNoiseTime = gpGlobals->curtime + VOICEBOX_NOISE_TIME;
		return;
	}

	// Awkwardly this has to be done here so we can make our attack noise above.
	if (m_fNextNoiseTime > gpGlobals->curtime)
		return;

	// We can use this to lie about not being angry while sneaking for stealth.
	bool Anger = m_bIsAngry;

	// Lie about our anger for a second.
	if (m_bIsSneaking)
		Anger = false;

	// TODO: add boomer and smoke voices.
	if (Anger)
	{
		switch (m_iSpecialType)
		{
			case VOICEBOX_TYPE_SMOKER:
				body->EmitSound("Smoker.AngryIdle");
				break;
			case VOICEBOX_TYPE_BOOMER:
				body->EmitSound("Boomer.AngryIdle");
				break;
			default:
				body->EmitSound("Zombie.AngryIdle");
		}
	}
	else
	{
		switch (m_iSpecialType)
		{
			case VOICEBOX_TYPE_SMOKER:
				body->EmitSound("Smoker.Idle");
				break;
			case VOICEBOX_TYPE_BOOMER:
				body->EmitSound("Boomer.Idle");
				break;
			default:
				body->EmitSound("Zombie.Idle");
		}
	}
	
	m_fNextNoiseTime = gpGlobals->curtime + VOICEBOX_NOISE_TIME + m_fNoiseOffset;
}