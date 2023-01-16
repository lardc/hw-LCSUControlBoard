// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "Global.h"

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//-----------------------------

void LL_LSLCurrentBoardLock(bool State)
{
	GPIO_SetState(GPIO_SYNC_CTRL, State);
}
//-----------------------------

void LL_Fan(bool State)
{
	GPIO_SetState(GPIO_FAN, State);
}
//-----------------------------

void LL_PowerSupplyEnable(bool State)
{
	GPIO_SetState(GPIO_PS_CTRL, State);
}
//-----------------------------

void LL_SetCurrentRange0()
{
	GPIO_SetState(GPIO_IFB_R0, false);
	GPIO_SetState(GPIO_IFB_R1, false);
}
//-----------------------------

void LL_SetCurrentRange1()
{
	GPIO_SetState(GPIO_IFB_R0, true);
	GPIO_SetState(GPIO_IFB_R1, false);
}
//-----------------------------

void LL_SetCurrentRange2()
{
	GPIO_SetState(GPIO_IFB_R0, false);
	GPIO_SetState(GPIO_IFB_R1, true);
}
//-----------------------------

void LL_WriteDAC(Int16U Data)
{
	DAC_SetValueCh1(DAC1, Data);
}
//-----------------------------

