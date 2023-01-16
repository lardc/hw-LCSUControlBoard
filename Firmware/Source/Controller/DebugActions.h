#ifndef __DEBUGACTIONS_H
#define __DEBUGACTIONS_H

// Include
//
#include "ZwBase.h"

// Functions
//
void DBGACT_SetCurrentRange(uint16_t Range);
void DBGACT_PowerSupplyEnable(bool State);
void DBGACT_FanControl(bool State);
void DBGACT_PulseProcess(Int16U DACValue);

#endif //__DEBUGACTIONS_H
