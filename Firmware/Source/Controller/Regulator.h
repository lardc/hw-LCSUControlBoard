#ifndef REGULATOR_H_
#define REGULATOR_H_

// Includes
//
#include "stdinc.h"
#include "Global.h"
#include "SysConfig.h"

// Definitions
//
#define PULSE_BUFFER_SIZE	VALUES_x_SIZE

// Structs
//
typedef struct __RegulatorParams
{
	Int16U CurrentRange;
	float CurrentTarget;
	float MeasuredCurrent;
	float MeasuredBatteryVoltage;
	float CurrentTable[PULSE_BUFFER_SIZE];
	float CurrentCorrectionTable[PULSE_BUFFER_SIZE];
	float Kp[CURRENT_RANGES];
	float Ki[CURRENT_RANGES];
	float KiTune[CURRENT_RANGES];
	float RegulatorError;
	bool DebugMode;
	Int16U PulseCounter;
	Int16U PulseCounterMax;
	Int16U ShelfIndex;
	float RegulatorOutput;
	Int16U DACOffset;
	Int16U DACLimitValue;
	Int16U DACSetpoint;
}RegulatorParamsStruct;

// Functions
//
bool REGULATOR_Process(volatile RegulatorParamsStruct* Regulator);
void REGULATOR_CashVariables(volatile RegulatorParamsStruct* Regulator);
//

#endif /* REGULATOR_H_ */
