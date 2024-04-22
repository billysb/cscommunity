//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========
//
// Purpose: Simple logical entity that counts up to a threshold value, then
//			fires an output when reached.
//
//=============================================================================

#include "cbase.h"
#include "cs_gamerules.h"

class CTerrorController : public CLogicalEntity
{
public:
	DECLARE_CLASS(CTerrorController, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CTerrorController()
	{
		m_nCounter = 0;
	}

	// Input function
	void InputTick(inputdata_t &inputData);

private:

	int	m_nThreshold;	// Count at which to fire our output
	int	m_nCounter;	// Internal counter

	COutputEvent	m_OnThreshold;	// Output event when the counter reaches the threshold
};

LINK_ENTITY_TO_CLASS(terror_controller, CTerrorController);

// Start of our data description for the class
BEGIN_DATADESC(CTerrorController)

// For save/load
DEFINE_FIELD(m_nCounter, FIELD_INTEGER),

// Links our member variable to our keyvalue from Hammer
DEFINE_KEYFIELD(m_nThreshold, FIELD_INTEGER, "threshold"),

// Links our input name from Hammer to our input member function
DEFINE_INPUTFUNC(FIELD_VOID, "Tick", InputTick),

// Links our output member to the output name used by Hammer
DEFINE_OUTPUT(m_OnThreshold, "OnThreshold"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Handle a tick input from another entity
//-----------------------------------------------------------------------------
void CTerrorController::InputTick(inputdata_t &inputData)
{
	// Increment our counter
	m_nCounter++;

	// See if we've met or crossed our threshold value
	if (m_nCounter >= m_nThreshold)
	{
		// Fire an output event
		m_OnThreshold.FireOutput(inputData.pActivator, this);

		// Reset our counter
		m_nCounter = 0;
	}
}