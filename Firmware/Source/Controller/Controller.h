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
	SS_None = 0,
	SS_PowerPrepare = 1,
	SS_PulsePrepare = 2,
	SS_WaitAfterPulse = 3,
	SS_Pulse = 4
} DeviceSubState;

// Variables
//
extern volatile DeviceState CONTROL_State;
extern volatile Int64U CONTROL_TimeCounter;
extern Int64U CONTROL_LEDTimeout;
extern volatile Int16U CONTROL_Values_Counter;
extern volatile Int16U CONTROL_RegulatorErr_Counter;
extern volatile float  CONTROL_ValuesCurrent[VALUES_x_SIZE];
extern volatile float  CONTROL_RegulatorErr[VALUES_x_SIZE];
extern volatile float  CONTROL_ValuesBatteryVoltage[VALUES_x_SIZE];
extern volatile float  CONTROL_RegulatorOutput[VALUES_x_SIZE];
extern volatile float  CONTROL_DACRawData[VALUES_x_SIZE];
//
extern volatile RegulatorParamsStruct RegulatorParams;


// Functions
//
void CONTROL_Init();
void CONTROL_Idle();
void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState);
void CONTROL_DelayMs(uint32_t Delay);
void CONTROL_HighPriorityProcess();
void CONTROL_ExternalInterruptProcess();
void CONTROL_SineShapeConfig(volatile RegulatorParamsStruct* Regulator);
void CONTROL_ModSineShapeConfig(volatile RegulatorParamsStruct* Regulator);
void CONTROL_TrapezeShapeConfig(volatile RegulatorParamsStruct* Regulator);
void CONTROL_PulseShapeConfig(volatile RegulatorParamsStruct* Regulator);
void CONTROL_CopyCurrentToEP(volatile RegulatorParamsStruct* Regulator);
void CONTROL_StartProcess();
void CONTROL_HandleFanLogic(bool IsImpulse);
void CONTROL_HandleExternalLamp(bool IsImpulse);

#endif // __CONTROLLER_H
