// Header
#include "DebugActions.h"

// Include
//
#include "LowLevel.h"
#include "Board.h"
#include "Delay.h"
#include "Controller.h"
#include "DataTable.h"
#include "Controller.h"
#include "Delay.h"

// Functions
//
void DBGACT_SetCurrentRange(uint16_t Range)
{
	Range ? LL_SetCurrentRange1() : LL_SetCurrentRange0();
}
//-----------------------------

void DBGACT_PowerSupplyEnable(bool State)
{
	LL_PowerSupplyEnable(State);
}
//-----------------------------

void DBGACT_FanControl(bool State)
{
	LL_Fan(State);
}
//-----------------------------

void DBGACT_ExtIndication(bool State)
{
	LL_ExtIndication(State);
}
//-----------------------------
