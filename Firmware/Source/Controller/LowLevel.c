// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "Global.h"
#include "InitConfig.h"

// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//-----------------------------

void LL_LSLCurrentBoardLock(bool State)
{
	GPIO_SetState(GPIO_OUT_LOCK, State);
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
	INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R0);
	GPIO_SetState(GPIO_CURRENT_RANGE, false);
}
//-----------------------------

void LL_SetCurrentRange1()
{
	INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R1);
	GPIO_SetState(GPIO_CURRENT_RANGE, true);
}
//-----------------------------

void LL_ExtIndication(bool State)
{
	GPIO_SetState(GPIO_IND_CTRL, State);
}
//-----------------------------

void LL_OutputAmplifierOffset(bool State)
{
	GPIO_SetState(GPIO_OFFSET_CTRL, State);
}
//-----------------------------

void LL_WriteDAC(Int16U Data)
{
	DAC_SetValueCh1(DAC1, Data);
}
//-----------------------------

