// Header
#include "LowLevel.h"
// Include
#include "Board.h"
#include "Delay.h"
#include "Global.h"
#include "InitConfig.h"
#include "DataTable.h"
#include "Constraints.h"


// Functions
//
void LL_ToggleBoardLED()
{
	GPIO_Toggle(GPIO_LED);
}
//-----------------------------

void LL_CurrentBoardLock(bool State)
{
	GPIO_SetState(GPIO_OUT_LOCK, !State);
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
	switch((Int16U)DataTable[REG_PCB_VERSION])
	{
		case PCB_VERSION_10:
			INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R0);
			break;

		case PCB_VERSION_11:
			INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R0);
			GPIO_SetState(GPIO_CURRENT_RANGE_SWITCH_PCB11, true);
			break;
	}
	INITCFG_ReloadCurrentADC_DMA();
	GPIO_SetState(GPIO_CURRENT_RANGE, false);
}
//-----------------------------

void LL_SetCurrentRange1()
{
	switch((Int16U)DataTable[REG_PCB_VERSION])
	{
		case PCB_VERSION_10:
			INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R1);
			break;

		case PCB_VERSION_11:
			INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R0);
			GPIO_SetState(GPIO_CURRENT_RANGE_SWITCH_PCB11, false);
			break;
	}
	INITCFG_ReloadCurrentADC_DMA();
	GPIO_SetState(GPIO_CURRENT_RANGE, true);
}
//-----------------------------

void LL_ExtIndication(bool State)
{
	GPIO_SetState(GPIO_IND_CTRL, State);
}
//-----------------------------

void LL_ToggleExtIndication()
{
	GPIO_Toggle(GPIO_IND_CTRL);
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

bool LL_SyncLineGetState()
{
	return GPIO_GetState(GPIO_SYNC);
}
//-----------------------------
