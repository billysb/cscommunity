//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "cs_shareddefs.h"


CCSClassInfo g_ClassInfos[] =
{
	{ "None" },

	{ "Phoenix Connection" },
	{ "L337 KREW" },
	{ "Arctic Avengers" },
	{ "Guerilla Warfare" },

	{ "Seal Team 6" },
	{ "GSG-9" },
	{ "SAS" },
	{ "GIGN" }
};

const CCSClassInfo* GetCSClassInfo( int i )
{
	Assert( i >= 0 && i < ARRAYSIZE( g_ClassInfos ) );
	return &g_ClassInfos[i];
}


// Construct some arrays of player model strings, so we can statically initialize CUtlVectors for general usage
const char *CTPlayerModelStrings[] =
{
	"models/player/ct_urban.mdl",
	"models/player/ct_gsg9.mdl",
	"models/player/ct_sas.mdl",
	"models/player/ct_gign.mdl",
};
const char *TerroristPlayerModelStrings[] =
{
	"models/player/t_phoenix.mdl",
	"models/player/t_leet.mdl",
	"models/player/t_arctic.mdl",
	"models/player/t_guerilla.mdl",
};

#ifdef SBTERROR
const char *SurvivorPlayerModelStrings[] =
{
	"models/player/gregrogers/l4d1/namvet/gregrogers.mdl",
	"models/player/gregrogers/l4d1/biker/gregrogers.mdl",
	"models/player/gregrogers/l4d1/manager/gregrogers.mdl",
	"models/player/gregrogers/l4d1/teenangst/gregrogers.mdl",
};
#endif

CUtlVectorInitialized< const char * > CTPlayerModels( CTPlayerModelStrings, ARRAYSIZE( CTPlayerModelStrings ) );
CUtlVectorInitialized< const char * > TerroristPlayerModels( TerroristPlayerModelStrings, ARRAYSIZE( TerroristPlayerModelStrings ) );
#ifdef SBTERROR
CUtlVectorInitialized< const char * > SurvivorPlayerModels( SurvivorPlayerModelStrings, ARRAYSIZE( SurvivorPlayerModelStrings ) );
#endif