// Header
//
#include "Regulator.h"
#include "DataTable.h"
#include "LowLevel.h"
#include "ConvertUtils.h"

// Functions prototypes
//
void REGULATOR_LoggingData(volatile RegulatorParamsStruct* Regulator);
Int16U REGULATOR_DACApplyLimits(float Value, Int16U Offset, Int16U LimitValue);

// Functions
//
bool REGULATOR_Process(volatile RegulatorParamsStruct* Regulator)
{
	static float Qi = 0, Qp;

	Regulator->RegulatorError = (Regulator->RegulatorPulseCounter == 0) ? 0 : (Regulator->CurrentTable[Regulator->RegulatorPulseCounter] - Regulator->MeasuredCurrent);

	Qp = Regulator->RegulatorError * Regulator->Kp[Regulator->CurrentRange];
	Qi += Regulator->RegulatorError * (Regulator->Ki[Regulator->CurrentRange] + Regulator->KiTune[Regulator->CurrentRange]);

	float Qi_max = (float)DataTable[REG_REGULATOR_QI_MAX];
	if(Qi > Qi_max)
		Qi = Qi_max;
	else if (Qi < -Qi_max)
		Qi = -Qi_max;

	Regulator->RegulatorOutput = Regulator->CurrentTable[Regulator->RegulatorPulseCounter] + Qp + Qi;

	// Выбор источника данных для записи в ЦАП
	float ValueToDAC;
	if(Regulator->DebugMode)
		ValueToDAC = Regulator->CurrentTable[Regulator->RegulatorPulseCounter];
	else
		ValueToDAC = CU_ItoDAC(Regulator->RegulatorOutput, Regulator->CurrentRange);

	// Проверка границ диапазона ЦАП
	Regulator->DACSetpoint = REGULATOR_DACApplyLimits(ValueToDAC, Regulator->DACOffset, Regulator->DACLimitValue);
	LL_WriteDAC(Regulator->DACSetpoint);

	REGULATOR_LoggingData(Regulator);
	Regulator->RegulatorPulseCounter++;
	if(Regulator->RegulatorPulseCounter >= PULSE_BUFFER_SIZE)
	{
		Regulator->DebugMode = false;
		Regulator->RegulatorPulseCounter = 0;
		Qi = 0;
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

		CONTROL_ValuesCurrent[LocalCounter] = (Int16U)(Regulator->MeasuredCurrent);
		CONTROL_RegulatorErr[LocalCounter] = (Int16S)(Regulator->RegulatorError);
		CONTROL_RegulatorOutput[LocalCounter] = (Int16S)(Regulator->RegulatorOutput);
		CONTROL_ValuesBatteryVoltage[LocalCounter] = (Int16U)(Regulator->MeasuredBatteryVoltage * 10);
		CONTROL_DACRawData[LocalCounter] = (Int16U)(Regulator->DACSetpoint);

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
	float CurrentMax = (float)DataTable[REG_CURRENT_PER_CURBOARD] / 10 * DataTable[REG_CURBOARD_QUANTITY];
	float CurrentTarget = (float)DataTable[REG_CURRENT_PULSE_VALUE] / 10;

	// Кеширование коэффициентов регулятора
	for(int i = 0; i < CURRENT_RANGE_QUANTITY; i++)
	{
		Regulator->Kp[i] = (float)DataTable[REG_REGULATOR_RANGE0_Kp + i * 2] / 1000;
		Regulator->Ki[i] = (float)DataTable[REG_REGULATOR_RANGE0_Ki + i * 2] / 1000;
		Regulator->KiTune[i] = (CurrentMax - CurrentTarget) * (float)DataTable[REG_REGULATOR_TF_Ki_RANG0 + i] / 1e6;
	}

	Regulator->DebugMode = false;
	Regulator->DACOffset = DataTable[REG_DAC_OFFSET];
	Regulator->DACLimitValue = (DAC_MAX_VAL > DataTable[REG_DAC_OUTPUT_LIMIT_VALUE]) ? \
			DataTable[REG_DAC_OUTPUT_LIMIT_VALUE] : DAC_MAX_VAL;
}
//-----------------------------------------------
