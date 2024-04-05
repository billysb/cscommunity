//hl2aitools

#include "cbase.h"

#include "sb_lua_handler.h"
#include "LuaBridge\LuaBridge.h"
#include "KeyValues.h"
#include "filesystem.h"
#include "cs_gamerules.h"
#include "cs_player.h"
#include "triggers.h"
#include "nav_area.h"
#include "nav_mesh.h"
#include "cs_bot.h"
#include <vector>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define LCOL Color(252, 186, 3)

MyLuaHandle* g_LuaHandle = NULL;
MyLuaHandle* GetLuaHandle()
{
	return g_LuaHandle;
}

MyLuaHandle* DestroyLuaHandle()
{
	Msg("[LUA] Destroying lua handler.\n");
	delete g_LuaHandle;
	g_LuaHandle = NULL;
	return NULL;
}

MyLuaHandle::MyLuaHandle() : LuaHandle()
{
	g_LuaHandle = this;
	Register();
}

void MyLuaHandle::Init()
{
	// I know this is stupid.
	this->RealInit(CSGameRules()->MapName());
}

void MyLuaHandle::RealInit(const char* mapFile)
{
	Msg("[LUA] Starting up.\n");
	
	// We wanna look for a matching .lua file to the .bsp name.
	const char *map = STRING(gpGlobals->mapname);
	const char *ext = ".lua";
	const char *prefix = "maps/";

	// Calculate the length of the concatenated string
	size_t prefix_len = strlen(prefix);
	size_t map_len = strlen(map);
	size_t ext_len = strlen(ext);
	size_t total_len = prefix_len + map_len + ext_len + 1; // +1 for the null terminator

	// Allocate memory for the concatenated string
	char *luaFile = new char[total_len];

	// Copy map to luaFile
	strcpy(luaFile, prefix);
	strcat(luaFile, map);

	// Concatenate ext to luaFile
	strcat(luaFile, ext);

	Msg("[LUA] Try to load: %s\n", luaFile);

	//Load into buffer
	FileHandle_t f = filesystem->Open(luaFile, "rb", "MOD");
	if (!f)
	{
		Msg("[LUA] Couldn't open filesystem for lua file, disabled!\n");
		return;
	}

	// load file into a null-terminated buffer
	int fileSize = filesystem->Size(f);
	unsigned bufSize = ((IFileSystem *)filesystem)->GetOptimalReadSize(f, fileSize + 1);

	char *buffer = (char*)((IFileSystem *)filesystem)->AllocOptimalReadBuffer(f, bufSize);
	Assert(buffer);

	((IFileSystem *)filesystem)->ReadEx(buffer, bufSize, fileSize, f); // read into local buffer
	buffer[fileSize] = '\0'; // null terminate file as EOF
	filesystem->Close(f);	// close file after reading

	int error = luaL_loadbuffer(GetLua(), buffer, fileSize, luaFile);
	if (error)
	{
		Warning("[LUA-ERR] %s\n", lua_tostring(GetLua(), -1));
		lua_pop(GetLua(), 1);  // pop error message from the stack 
		Warning("[LUA-ERR] One or more errors occured while loading lua script!\n");
		return;
	}
	CallLUA(GetLua(), 0, LUA_MULTRET, 0, luaFile);
	m_bLuaLoaded = true;
	delete[] luaFile;
}

void MyLuaHandle::RegGlobals()
{
	if (!GetLua())
		return;

	LG_DEFINE_INT("FOR_ALL_PLAYERS", -1);
	LG_DEFINE_INT("INVALID_ENTITY", -1);
	LG_DEFINE_INT("NULL", 0);
	//LG_DEFINE_INT("GE_MAX_HEALTH", MAX_HEALTH);
	//LG_DEFINE_INT("GE_MAX_ARMOR", MAX_ARMOR);
	LG_DEFINE_INT("MAX_PLAYERS", gpGlobals->maxClients);

	//Team Indices
	LG_DEFINE_INT("TEAM_NONE", TEAM_UNASSIGNED);
	LG_DEFINE_INT("TEAM_SPECTATOR", TEAM_SPECTATOR);
	LG_DEFINE_INT("TEAM_TERRORIST", TEAM_TERRORIST);
	LG_DEFINE_INT("TEAM_SURVIVOR", TEAM_TERRORIST);
	LG_DEFINE_INT("TEAM_CT", TEAM_CT);
	LG_DEFINE_INT("TEAM_ZOMBIE", TEAM_CT);


	//ClientPrintAll Types
	LG_DEFINE_INT("HUD_PRINTNOTIFY", HUD_PRINTNOTIFY);
	LG_DEFINE_INT("HUD_PRINTCONSOLE", HUD_PRINTCONSOLE);
	LG_DEFINE_INT("HUD_PRINTTALK", HUD_PRINTTALK);
	LG_DEFINE_INT("HUD_PRINTCENTER", HUD_PRINTCENTER);
}

void MyLuaHandle::RegFunctions()
{
	if (!GetLua())
		return;
	REG_FUNCTION(Msg);
	REG_FUNCTION(ConMsg);
	REG_FUNCTION(ClientPrintAll);
	REG_FUNCTION(GetTime);
}


void MyLuaHandle::PassObjects()
{
	if (!GetLua())
		return;

	luabridge::getGlobalNamespace(GetLua())
		.beginClass<QAngle>("QAngle")
		.addFunction("Init", &QAngle::Init)
		.addFunction("Invalidate", &QAngle::Invalidate)
		.addFunction("IsValid", &QAngle::IsValid)
		.addFunction("Length", &QAngle::Length)
		.addFunction("LengthSqr", &QAngle::LengthSqr)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<Vector>("Vector")
		.addConstructor<void(*)(float, float, float)>()
		.addFunction("Cross", &Vector::Cross)
		.addFunction("DistTo", &Vector::DistTo)
		.addFunction("DistToSqr", &Vector::DistToSqr)
		.addFunction("Dot", &Vector::Dot)
		.addFunction("Init", &Vector::Init)
		.addFunction("Normalized", &Vector::Normalized)
		.addFunction("IsValid", &Vector::IsValid)
		.addFunction("IsZero", &Vector::IsZero)
		.addFunction("Max", &Vector::Max)
		.addFunction("Min", &Vector::Min)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CCSBot>("CCSBot")
		.addFunction("MoveTo", &CCSBot::MoveTo)
		.addFunction("BecomeAlert", &CCSBot::BecomeAlert)
		.addFunction("Attack", &CCSBot::Attack)
		.addFunction("IsAttacking", &CCSBot::IsAttacking)
		.addFunction("IsAlert", &CCSBot::IsAlert)
		.addFunction("IsSneaking", &CCSBot::IsSneaking)
		.addFunction("IsAtBombsite", &CCSBot::IsAtBombsite)
		.addFunction("IsAtEnemySpawn", &CCSBot::IsAtEnemySpawn)
		.addFunction("IsRogue", &CCSBot::IsRogue)
		.addFunction("Reload", &CCSBot::Reload)
		.addFunction("IsReloading", &CCSBot::IsReloading)
		.addFunction("StopAttacking", &CCSBot::StopAttacking)
		.addFunction("EquipBestWeapon", &CCSBot::EquipBestWeapon)
		.addFunction("EquipGrenade", &CCSBot::EquipGrenade)
		.addFunction("EquipKnife", &CCSBot::EquipKnife)
		.addFunction("EquipKnife", &CCSBot::EquipPistol)
		.addFunction("ForgetNoise", &CCSBot::ForgetNoise)
		.addFunction("StopAiming", &CCSBot::StopAiming)
		.addFunction("StopFollowing", &CCSBot::StopFollowing)
		.addFunction("StopFollowingEntity", &CCSBot::StopFollowingEntity)
		.addFunction("Panic", &CCSBot::Panic)
		.addFunction("IsPanicking", &CCSBot::IsPanicking)
		.addFunction("ClearLookAt", &CCSBot::ClearLookAt)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CCSPlayer>("CCSPlayer")
		.addFunction("ArmorValue", &CCSPlayer::ArmorValue)
		.addFunction("CanPlayerBuy", &CCSPlayer::CanPlayerBuy)
		.addFunction("GetTeamNumber", &CCSPlayer::GetTeamNumber)
		.addFunction("GetHealth", &CCSPlayer::GetHealth)
		.addFunction("SetHealth", &CCSPlayer::SetHealth)
		.addFunction("SetGravity", &CCSPlayer::SetGravity)
		.addFunction("GetGravity", &CCSPlayer::GetGravity)
		.addFunction("AcceptInput", &CCSPlayer::AcceptInput)
		.addFunction("SetModel", &CCSPlayer::SetModel)
		.addFunction("GetModelName", &CCSPlayer::GetModelName)
		.addFunction("GetAbsAngles", &CCSPlayer::GetAbsAngles)
		.addFunction("GetAbsOrigin", &CCSPlayer::GetAbsOrigin)
		.addFunction("GetAbsVelocity", &CCSPlayer::GetAbsVelocity)
		.addFunction("GetAliveDuration", &CCSPlayer::GetAliveDuration)
		.addFunction("Weapon_DropAll", &CCSPlayer::Weapon_DropAll)
		.addFunction("SetAbsAngles", &CCSPlayer::SetAbsAngles)
		.addFunction("SetAbsOrigin", &CCSPlayer::SetAbsOrigin)
		.addFunction("SetAbsVelocity", &CCSPlayer::SetAbsVelocity)
		.addFunction("SetArmorValue", &CCSPlayer::SetArmorValue)
		.addFunction("RemoveAllAmmo", &CCSPlayer::RemoveAllAmmo)
		.addFunction("Remove", &CCSPlayer::RemoveAllWeapons)
		.addFunction("CanMove", &CCSPlayer::CanMove)
		.addFunction("IsDead", &CCSPlayer::IsDead)
		.addFunction("IsAlive", &CCSPlayer::IsAlive)
		.addFunction("InSameTeam", &CCSPlayer::InSameTeam)
		.addFunction("GetWaterLevel", &CCSPlayer::GetWaterLevel)
		.addFunction("GetLastKnownArea", &CCSPlayer::GetLastKnownArea)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CBasePlayer>("CBasePlayer")
		.addFunction("ArmorValue", &CBasePlayer::ArmorValue)
		.addFunction("GetTeamNumber", &CBasePlayer::GetTeamNumber)
		.addFunction("GetHealth", &CBasePlayer::GetHealth)
		.addFunction("SetHealth", &CBasePlayer::SetHealth)
		.addFunction("SetGravity", &CBasePlayer::SetGravity)
		.addFunction("GetGravity", &CBasePlayer::GetGravity)
		.addFunction("AcceptInput", &CBasePlayer::AcceptInput)
		.addFunction("SetModel", &CBasePlayer::SetModel)
		.addFunction("GetModelName", &CBasePlayer::GetModelName)
		.addFunction("GetAbsAngles", &CBasePlayer::GetAbsAngles)
		.addFunction("GetAbsOrigin", &CBasePlayer::GetAbsOrigin)
		.addFunction("GetAbsVelocity", &CBasePlayer::GetAbsVelocity)
		.addFunction("GetAliveDuration", &CBasePlayer::GetAliveDuration)
		.addFunction("Weapon_DropAll", &CBasePlayer::Weapon_DropAll)
		.addFunction("SetAbsAngles", &CBasePlayer::SetAbsAngles)
		.addFunction("SetAbsOrigin", &CBasePlayer::SetAbsOrigin)
		.addFunction("SetAbsVelocity", &CBasePlayer::SetAbsVelocity)
		.addFunction("SetArmorValue", &CBasePlayer::SetArmorValue)
		.addFunction("RemoveAllAmmo", &CBasePlayer::RemoveAllAmmo)
		.addFunction("Remove", &CBasePlayer::RemoveAllWeapons)
		.addFunction("IsDead", &CBasePlayer::IsDead)
		.addFunction("IsAlive", &CBasePlayer::IsAlive)
		.addFunction("IsBot", &CBasePlayer::IsBot)
		.addFunction("InSameTeam", &CBasePlayer::InSameTeam)
		.addFunction("GetWaterLevel", &CBasePlayer::GetWaterLevel)
		.addFunction("GetLastKnownArea", &CBasePlayer::GetLastKnownArea)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CCSGameRules>("CCSGameRules")
		.addFunction("BalanceTeams", &CCSGameRules::BalanceTeams)
		.addFunction("BroadcastSound", &CCSGameRules::BroadcastSound)
		.addFunction("CleanUpMap", &CCSGameRules::CleanUpMap)
		.addFunction("IsBombDefuseMap", &CCSGameRules::IsBombDefuseMap)
#ifdef TERROR
		.addFunction("IsTerrorStrike", &CCSGameRules::IsTerrorStrikeMap)
		.addFunction("SetTerrorStrike", &CCSGameRules::SetTerrorMode)
		.addFunction("SetWaveTime", &CCSGameRules::ForceZombieWaveTime)
#endif
		.addFunction("GetMapElapsedTime", &CCSGameRules::GetMapElapsedTime)
		.addFunction("GetMapRemainingTime", &CCSGameRules::GetMapRemainingTime)
		//.addFunction("GetSpawnBasedOnEnt", &CCSGameRules::GetSpawnBasedOnEnt)
		//.addFunction("IsIntermission", &CCSGameRules::IsIntermission)
		.addFunction("IsBuyTimeElapsed", &CCSGameRules::IsBuyTimeElapsed)
		.addFunction("IsFreezePeriod", &CCSGameRules::IsFreezePeriod)
		.addFunction("IsFriendlyFireOn", &CCSGameRules::IsFriendlyFireOn)
#ifdef TERROR
		.addFunction("IsNavVisibleBySurvivors", &CCSGameRules::IsNavVisibleBySurvivors)
#endif
		.endClass();
	
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CNavMesh>("TheNavmesh")
		.addFunction("BeginAnalysis", &CNavMesh::BeginAnalysis)
		.addFunction("BeginGeneration", &CNavMesh::BeginGeneration)
		.addFunction("GetNavArea", static_cast<CNavArea* (CNavMesh::*)(const Vector&, float) const>(&CNavMesh::GetNavArea))
		.addFunction("GetNavArea", static_cast<CNavArea* (CNavMesh::*)(CBaseEntity*, int, float) const>(&CNavMesh::GetNavArea))
		.endClass();
		
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CBaseEntity>("CBaseEntity")
		.addFunction("SetModel", &CBaseEntity::SetModel)
		.addFunction("GetModelName", &CBaseEntity::GetModelName)
		.addFunction("AcceptInput", &CBaseEntity::AcceptInput)
		.addFunction("IsPlayer", &CBaseEntity::IsPlayer)
		.addFunction("IsAlive", &CBaseEntity::IsAlive)
		.addFunction("IsNPC", &CBaseEntity::IsNPC)
		.addFunction("InSameTeam", &CBaseEntity::InSameTeam)
		.addFunction("GetHealth", &CBaseEntity::GetHealth)
		.addFunction("SetHealth", &CBaseEntity::SetHealth)
		.addFunction("GetAbsAngles", &CBaseEntity::GetAbsAngles)
		.addFunction("SetAbsAngles", &CBaseEntity::SetAbsAngles)
		.addFunction("GetAbsOrigin", &CBaseEntity::GetAbsOrigin)
		.addFunction("SetAbsOrigin", &CBaseEntity::SetAbsOrigin)
		.addFunction("GetAbsVelocity", &CBaseEntity::GetAbsVelocity)
		.addFunction("SetAbsVelocity", &CBaseEntity::SetAbsVelocity)
		.addFunction("GetEntityName", &CBaseEntity::GetEntityName)
		.addFunction("GetWaterLevel", &CBaseEntity::GetWaterLevel)
		.addFunction("GetClassname", &CBaseEntity::GetClassname)
		.addFunction("GetEntityName", &CBaseEntity::GetEntityName)
		.endClass();

	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CBaseTrigger>("CBaseTrigger")
		.addFunction("SetModel", &CBaseTrigger::SetModel)
		.addFunction("GetModelName", &CBaseTrigger::GetModelName)
		.addFunction("AcceptInput", &CBaseTrigger::AcceptInput)
		.addFunction("IsPlayer", &CBaseTrigger::IsPlayer)
		.addFunction("IsAlive", &CBaseTrigger::IsAlive)
		.addFunction("IsNPC", &CBaseTrigger::IsNPC)
		.addFunction("InSameTeam", &CBaseTrigger::InSameTeam)
		.addFunction("GetHealth", &CBaseTrigger::GetHealth)
		.addFunction("SetHealth", &CBaseTrigger::SetHealth)
		.addFunction("GetAbsAngles", &CBaseTrigger::GetAbsAngles)
		.addFunction("SetAbsAngles", &CBaseTrigger::SetAbsAngles)
		.addFunction("GetAbsOrigin", &CBaseTrigger::GetAbsOrigin)
		.addFunction("SetAbsOrigin", &CBaseTrigger::SetAbsOrigin)
		.addFunction("GetAbsVelocity", &CBaseTrigger::GetAbsVelocity)
		.addFunction("SetAbsVelocity", &CBaseTrigger::SetAbsVelocity)
		.addFunction("GetEntityName", &CBaseTrigger::GetEntityName)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CNavArea>("CNavArea")
		.addFunction("GetCenter", &CNavArea::GetCenter)
		.addFunction("GetDanger", &CNavArea::GetDanger)
		.addFunction("GetRandomPoint", &CNavArea::GetRandomPoint)
		.addFunction("GetPlayerCount", &CNavArea::GetPlayerCount)
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<CGlobalEntityList>("CGlobablEntityList") // Broken
		.endClass();
	luabridge::getGlobalNamespace(GetLua())
		.beginClass<string_t>("string_t")
		.addFunction("ToCStr", &string_t::ToCStr)
		.endClass();

	// functions.
	luabridge::getGlobalNamespace(GetLua())
		.addFunction("EdictToPlayer", &UTIL_PlayerByIndex) // Broken?
		.addFunction("EntToPlayer", ToCSPlayer)
		//.addFunction("PlayerToBot", &PlayerToBot)
		//.addFunction("PlayerToBot", static_cast<CCSBot*(*)(CCSPlayer*)>(&MyLuaHandle::PlayerToBot));
		.addFunction("GetEntitiesByName", GetEntitiesByName); // FIGURE OUT WHY THIS LUABIND DONT WORK
	
	CCSGameRules *test = CSGameRules();

	luabridge::push(GetLua(), test);
	lua_setglobal(GetLua(), "GameRules");

	luabridge::push(GetLua(), TheNavMesh);
	lua_setglobal(GetLua(), "TheNavMesh");

	//luabridge::push(GetLua(), gEntList);
	//lua_setglobal(GetLua(), "gEntList");
}

void MyLuaHandle::Shutdown()
{
}

inline CCSBot* PlayerToBot(CBasePlayer *pPlayer)
{
	CCSBot* pBot = dynamic_cast<CCSBot*>(pPlayer);
	return pBot;
}

void GetEntitiesByName(const char *name) {
	MyLuaHandle *lh = GetLuaHandle();
	if (!lh)
		return;

	lua_State *L = lh->GetLua();

	// Create a Lua table
	lua_newtable(L);

	int Found = 0;

	// Search for entities with the given name
	CBaseEntity *pCur = gEntList.FindEntityByClassname(nullptr, name);

	while (pCur)
	{
		if (Found > 100)
			break;

		if (strcmp(pCur->GetClassname(), name) == 0)
		{
			// Increment Found
			Found++;

			// Push Found as key
			lua_pushinteger(L, Found);  // Lua tables are 1-indexed

			// Push the CBaseEntity* as userdata or lightuserdata
			lua_pushlightuserdata(L, pCur);

			// Set the value at the current index
			lua_settable(L, -3);  // -3 is the index of the Lua table on the stack
		}

		pCur = gEntList.FindEntityByClassname(pCur, name);
	}

	// Duplicate the Lua table at the top of the stack
	lua_pushvalue(L, -1);

	// Clean up the stack if necessary
	// (e.g., if you pushed additional values onto the stack)
	// ...

	// Return the Lua table
	return;
}

// Wrapped functions / helpers.

int luaClientPrintAll(lua_State *L)
{
	int n = lua_gettop(L);    /* number of arguments */
	switch (n)
	{
	case 2:
		UTIL_ClientPrintAll(lua_tointeger(L, 1), lua_tostring(L, 2));
		break;
	case 3:
		UTIL_ClientPrintAll(lua_tointeger(L, 1), lua_tostring(L, 2), lua_tostring(L, 3));
		break;
	case 4:
		UTIL_ClientPrintAll(lua_tointeger(L, 1), lua_tostring(L, 2), lua_tostring(L, 3), lua_tostring(L, 4));
		break;
	case 5:
		UTIL_ClientPrintAll(lua_tointeger(L, 1), lua_tostring(L, 2), lua_tostring(L, 3), lua_tostring(L, 4), lua_tostring(L, 5));
		break;
	case 6:
		UTIL_ClientPrintAll(lua_tointeger(L, 1), lua_tostring(L, 2), lua_tostring(L, 3), lua_tostring(L, 4), lua_tostring(L, 5), lua_tostring(L, 6));
		break;
	}
	return 0;
}

int luaMsg(lua_State *L)
{
	Msg("%s\n", lua_tostring(L, 1));
	return 0;
}

int luaConMsg(lua_State *L)
{
	return luaMsg(L);
}

int luaGetTime(lua_State *L)
{
	lua_pushnumber(L, gpGlobals->curtime);
	return 1;
}