#ifndef SBTOOL_H
#define SBTOOL_H
#ifdef _WIN32
#pragma once
#endif

/*
So you are probably wonder what this is.
For my personally built binaries this tool will handle data collection for the optional bugreporter.

This is never sent automatically. These files will be created for the user to send, never automatically against the users wishes.
*/

class SBtool
{
private:
	float m_fSessionStart;
	bool m_bIsStarted;
public:
	SBtool();
	~SBtool();
};

#endif