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
	float Kp[CURRENT_RANGE_QUANTITY];
	float Ki[CURRENT_RANGE_QUANTITY];
	float KiTune[CURRENT_RANGE_QUANTITY];
	float RegulatorError;
	bool DebugMode;
	Int16U RegulatorPulseCounter;
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
