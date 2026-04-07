#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
//
#include "stdinc.h"
#include "Global.h"
#include "Regulator.h"

// Types
//
typedef enum __DeviceState
{
	DS_None = 0,
	DS_Fault = 1,
	DS_Disabled = 2,
	DS_Ready = 3,
	DS_ConfigReady = 4,
	DS_InProcess = 5
} DeviceState;

typedef enum __DeviceSubState
{
	SS_None,
	SS_PowerPrepare,
	SS_PulsePrepare,
	SS_PulsePrepareDelay,
	SS_WaitAfterPulse,
	SS_Pulse
} DeviceSubState;

// Variables
//
extern volatile DeviceState CONTROL_State;
extern volatile Int64U CONTROL_TimeCounter;
extern Int64U CONTROL_LEDTimeout;
extern volatile Int16U CONTROL_Values_Counter;
extern volatile Int16U CONTROL_RegulatorErr_Counter;
extern volatile Int16U CONTROL_ExtInfoCounter;
extern volatile float  CONTROL_ValuesCurrent[];
extern volatile float  CONTROL_RegulatorErr[];
extern volatile float  CONTROL_ValuesBatteryVoltage[];
extern volatile float  CONTROL_RegulatorOutput[];
extern volatile float  CONTROL_CurentTable[];
extern volatile float  CONTROL_DACRawData[];
//
extern volatile float CONTROL_ExtInfoData[VALUES_EXT_INFO_SIZE];

// Functions
//
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_HighPriorityProcess();
void CONTROL_ExternalInterruptProcess();
Int16U CONTROL_GetCurrentRange();
void CONTROL_StartProcess();
void CONTROL_StopProcess();
void CONTROL_HandleFanLogic(bool IsImpulse);
void CONTROL_HandleExternalLamp(bool IsImpulse);
void CONTROL_InitJSONPointers();

#endif // __CONTROLLER_H
