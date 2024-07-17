// Header
//
#include "Regulator.h"
#include "DataTable.h"
#include "LowLevel.h"
#include "ConvertUtils.h"
#include "Controller.h"
#include "Math.h"

// Functions prototypes
//
void REGULATOR_LoggingData(volatile RegulatorParamsStruct* Regulator);
Int16U REGULATOR_DACApplyLimits(float Value, Int16U Offset, Int16U LimitValue);

// Functions
//
bool REGULATOR_Process(volatile RegulatorParamsStruct* Regulator)
{
	static float Qi = 0, Qp;
	static Int16U  FollowingErrorCounter = 0;
	Regulator->RegulatorError = (Regulator->PulseCounter <= Regulator->PlateIndex) ? 0 : (Regulator->CurrentTable[Regulator->PulseCounter] - Regulator->MeasuredCurrent);

	if(fabsf(Regulator->RegulatorError / Regulator->CurrentTarget * 100) < Regulator->RegulatorAlowedError)
		FollowingErrorCounter = 0;
	else
		FollowingErrorCounter++;

	if(FollowingErrorCounter >= Regulator->FollowingErrorCounterMax && !DataTable[REG_FOLLOWING_ERR_MUTE])
	{
		FollowingErrorCounter = 0;
		CONTROL_StopProcess();
		CONTROL_SetDeviceState(DS_Ready, SS_None);
		DataTable[REG_PROBLEM] = PROBLEM_FOLLOWING_ERROR;
	}

	Qp = Regulator->RegulatorError * Regulator->Kp[Regulator->CurrentRange];
	Qi += Regulator->RegulatorError * (Regulator->Ki[Regulator->CurrentRange] + Regulator->KiTune[Regulator->CurrentRange]);

	if(Qi > DataTable[REG_REGULATOR_QI_MAX])
		Qi = DataTable[REG_REGULATOR_QI_MAX];
	else if (Qi < -DataTable[REG_REGULATOR_QI_MAX])
		Qi = -DataTable[REG_REGULATOR_QI_MAX];

	Regulator->RegulatorOutput = Regulator->CurrentCorrectionTable[Regulator->PulseCounter] + Qp + Qi;

	// Выбор источника данных для записи в ЦАП
	float ValueToDAC;
	if(Regulator->DebugMode)
		ValueToDAC = Regulator->CurrentTable[Regulator->PulseCounter];
	else
		ValueToDAC = CU_ItoDAC(Regulator->RegulatorOutput, Regulator->CurrentRange);

	// Проверка границ диапазона ЦАП
	Regulator->DACSetpoint = REGULATOR_DACApplyLimits(ValueToDAC, Regulator->DACOffset, Regulator->DACLimitValue);
	LL_WriteDAC(Regulator->DACSetpoint);

	if (Regulator->PulseCounter == Regulator->PulseCounterMax/2)
		DataTable[REG_RESULT_CURRENT] = Regulator->MeasuredCurrent;
	REGULATOR_LoggingData(Regulator);
	Regulator->PulseCounter++;
	if(Regulator->PulseCounter >= Regulator->PulseCounterMax || DataTable[REG_PROBLEM] == PROBLEM_FOLLOWING_ERROR)
	{
		Regulator->RegulatorError = 0;
		Regulator->DebugMode = false;
		Regulator->PulseCounter = 0;
		Qi = 0;
		FollowingErrorCounter = 0;
		return true;
	}
	else
		return false;
}
//-----------------------------------------------

Int16U REGULATOR_DACApplyLimits(float Value, Int16U Offset, Int16U LimitValue)
{
	Int16S Result = (Int16S)(Value + Offset);
	if (Result < 0)
		return 0;
	else if (Result > LimitValue)
		return LimitValue;
	else
		return Result;
}
//-----------------------------------------------

void REGULATOR_LoggingData(volatile RegulatorParamsStruct* Regulator)
{
	static Int16U ScopeLogStep = 0, LocalCounter = 0;

	// Сброс локального счетчика в начале логгирования
	if (CONTROL_Values_Counter == 0)
		LocalCounter = 0;

	if (ScopeLogStep++ >= DataTable[REG_SCOPE_STEP])
	{
		ScopeLogStep = 0;

		CONTROL_ValuesCurrent[LocalCounter] = Regulator->MeasuredCurrent;
		CONTROL_RegulatorErr[LocalCounter] = Regulator->RegulatorError;
		CONTROL_RegulatorOutput[LocalCounter] = Regulator->RegulatorOutput;
		CONTROL_ValuesBatteryVoltage[LocalCounter] = Regulator->MeasuredBatteryVoltage;
		CONTROL_DACRawData[LocalCounter] = Regulator->DACSetpoint;

		CONTROL_Values_Counter = LocalCounter;

		++LocalCounter;
	}

	// Условие обновления глобального счетчика данных
	if (CONTROL_Values_Counter < VALUES_x_SIZE)
		CONTROL_Values_Counter = LocalCounter;

	// Сброс локального счетчика
	if (LocalCounter >= VALUES_x_SIZE)
		LocalCounter = 0;
}
//-----------------------------------------------

void REGULATOR_CashVariables(volatile RegulatorParamsStruct* Regulator)
{
	float CurrentMax = DataTable[REG_CURRENT_PER_CURBOARD] * DataTable[REG_CURBOARDS];
	float CurrentTarget = DataTable[REG_CURRENT_PULSE_VALUE];

	// Кеширование коэффициентов регулятора
	for(int i = 0; i < CURRENT_RANGES; i++)
	{
		Regulator->Kp[i] = DataTable[REG_REGULATOR_RANGE0_Kp + i * 2];
		Regulator->Ki[i] = DataTable[REG_REGULATOR_RANGE0_Ki + i * 2];
		Regulator->KiTune[i] = (CurrentMax - CurrentTarget) * DataTable[REG_REGULATOR_TF_Ki_RANG0 + i];
	}

	Regulator->DebugMode = false;
	Regulator->DACOffset = DataTable[REG_DAC_OFFSET];
	Regulator->DACLimitValue = (DAC_MAX_VAL > DataTable[REG_DAC_OUTPUT_LIMIT_VALUE]) ? \
			DataTable[REG_DAC_OUTPUT_LIMIT_VALUE] : DAC_MAX_VAL;
	Regulator->PulseCounter = 0;
	Regulator->RegulatorAlowedError = DataTable[REG_REGULATOR_ALLOWED_ERR];
	Regulator->FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];
}
//-----------------------------------------------
