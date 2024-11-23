#ifndef SBZOMBIEBOX_H
#define SBZOMBIEBOX_H

#ifdef _WIN32
#pragma once
#endif

//#include "cs_player.h"
//#include "in_buttons.h"

class CCSPlayer;

class CZVoiceBox
{
public:

	CZVoiceBox(CCSPlayer* me);


	void GetHumanName(void);

	void SpawnNoise(void);
	void DeathNoise(void);
	void PainNoise(void);

	// Called when attacking.
	void AttackNoise(void);

	// For special infected shit.
	void ForceUpdate(void);

	// "watch" player for changes.
	void Update(void);

	// where we make the sounds.
	void Think(void);


private:

	CCSPlayer* m_hOwner;

	// human stuff.
	char* character;

	// Am I human?
	bool m_bIsHuman;

	// Represent how bad health is for specific health related lines.
	short m_sHealthState;
	
	float m_fLastAttackerTime;

	// Humans will track how many attackers surround them
	int m_attackers;

	bool	m_bIsAngry;
	bool	m_bIsSpecial;
	bool	m_bIsOnFire;
	int		m_iSpecialType;

	bool	m_bIsSneaking;

	float	m_fLastAttackTime;

	float	m_fNextUpdateTime;

	// When will we check if we should still be angry.
	float	m_fNextAngerTime;

	// How long until our next idle noise.
	float	m_fNextNoiseTime;

	// When on fire we dont want to spam pain sounds.
	float m_fNextScreamTime;

	// This is needed to stop bots from all doing shit at the same time.
	float	m_fUpdateOffset;
	float	m_fNoiseOffset;
};

#endif
