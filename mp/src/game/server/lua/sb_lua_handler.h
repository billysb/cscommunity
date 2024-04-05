// Credit to SecretiveLoon for providing examples.
#ifndef MYLUAMANAGER_H
#define MYLUAMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "ge_luamanager.h"

class MyLuaHandle : public LuaHandle
{
public:
	void Init();
	void RealInit(const char* mapFile); // stupid but works.
	void Shutdown(void);

	void RegFunctions(void);
	void RegGlobals(void);
	void PassObjects(void);

	MyLuaHandle(void);

	// helper

private:
	bool m_bLuaLoaded; 
};

extern MyLuaHandle* GetLuaHandle();
extern MyLuaHandle* DestroyLuaHandle();

extern void GetEntitiesByName(const char *name);
#endif //MC_GE_LUAMANAGER_H
