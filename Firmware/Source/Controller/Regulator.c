// Header
//
#include "Regulator.h"
#include "DataTable.h"
#include "LowLevel.h"
#include "ConvertUtils.h"
#include "Controller.h"
#include "Math.h"
#include "Measurement.h"

// Variabls
//
static Int16U DACLimitValue, FollowingErrorCounterMax;
static float RegulatorAlowedError, Kp, Ki, KiTune;
static bool DisableRegulator;

float CurrentTable[VALUES_x_SIZE];
float CurrentCorrectionTable[VALUES_x_SIZE];
Int16U PulseCounter;
Int16U PulseCounterMax;

// Functions prototypes
//
void REGULATOR_LoggingData(float MeasuredCurrent, float MeasuredBatteryVoltage, float RegulatorOutput,
		float RegulatorError, float Setpoint, float DACValue);
Int16U REGULATOR_DACApplyLimits(float Value, Int16U LimitValue);

// Functions
//
bool REGULATOR_Process()
{
	static float Qi = 0;
	static Int16U FollowingErrorCounter = 0;
	float RegulatorError, RegulatorRelativeError;

	// Получение измеренных значений
	float MeasuredCurrent, MeasuredBatteryVoltage;
	MEASURE_SampleParams(&MeasuredCurrent, &MeasuredBatteryVoltage);

	// На нулевом тике ошибку не из чего рассчитать
	// Так же сбрасываются статические переменные
	if(PulseCounter == 0)
	{
		RegulatorError = 0;
		RegulatorRelativeError = 0;

		Qi = 0;
		FollowingErrorCounter = 0;
	}
	else
	{
		// Для ошибки берётся предыдущее задание
		RegulatorError = CurrentTable[PulseCounter - 1] - MeasuredCurrent;
		RegulatorRelativeError = RegulatorError / CurrentTable[PulseCounter - 1];
	}

	// Проверка Following Error
	if(!DataTable[REG_FOLLOWING_ERR_MUTE] && !DisableRegulator)
	{
		if(fabsf(RegulatorRelativeError * 100) < RegulatorAlowedError)
			FollowingErrorCounter = 0;
		else
			FollowingErrorCounter++;

		if(FollowingErrorCounter >= FollowingErrorCounterMax)
		{
			DataTable[REG_PROBLEM] = PROBLEM_FOLLOWING_ERROR;
			return true;
		}
	}

	// Расчёт корректировок
	float Qp = RegulatorError * Kp;
	Qi += RegulatorError * (Ki + KiTune);

	if(Qi > DataTable[REG_REGULATOR_QI_MAX])
		Qi = DataTable[REG_REGULATOR_QI_MAX];
	else if (Qi < -DataTable[REG_REGULATOR_QI_MAX])
		Qi = -DataTable[REG_REGULATOR_QI_MAX];

	// Скорректированное значение
	float RegulatorOutput = CurrentCorrectionTable[PulseCounter]
			+ (DisableRegulator ? 0 : (Qp + Qi));

	// Пересчёт в ЦАП
	float ValueToDAC = CU_ItoDAC(RegulatorOutput);
	Int16U DACSetpoint = REGULATOR_DACApplyLimits(ValueToDAC, DACLimitValue);
	LL_WriteDAC(DACSetpoint);

	REGULATOR_LoggingData(MeasuredCurrent, MeasuredBatteryVoltage, RegulatorOutput,
			RegulatorError, 0, DACSetpoint);
	PulseCounter++;

	return (PulseCounter >= PulseCounterMax);
}
//-----------------------------------------------

Int16U REGULATOR_DACApplyLimits(float Value, Int16U LimitValue)
{
	if (Value < 0)
		return 0;
	else if (Value > LimitValue)
		return LimitValue;
	else
		return Value;
}
//-----------------------------------------------

void REGULATOR_LoggingData(float MeasuredCurrent, float MeasuredBatteryVoltage, float RegulatorOutput,
		float RegulatorError, float Setpoint, float DACValue)
{
	static Int16U ScopeLogStep = 0, LocalCounter = 0;

	// Сброс локального счетчика в начале логгирования
	if (CONTROL_Values_Counter == 0)
		LocalCounter = 0;

	if (ScopeLogStep++ >= DataTable[REG_SCOPE_STEP])
	{
		ScopeLogStep = 0;

		CONTROL_ValuesCurrent[LocalCounter] = MeasuredCurrent;
		CONTROL_ValuesBatteryVoltage[LocalCounter] = MeasuredBatteryVoltage;
		CONTROL_RegulatorOutput[LocalCounter] = RegulatorOutput;
		CONTROL_RegulatorErr[LocalCounter] = RegulatorError;
		CONTROL_CurentTable[LocalCounter] = Setpoint;
		CONTROL_DACRawData[LocalCounter] = DACValue;

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

void REGULATOR_CashVariables()
{
	float CurrentMax = DataTable[REG_CURRENT_PER_CURBOARD] * DataTable[REG_CURBOARDS];
	float CurrentTarget = DataTable[REG_CURRENT_PULSE_VALUE];

	// Кеширование коэффициентов регулятора
	Int16U CurrentRange = CONTROL_GetCurrentRange();
	if(CURRENT_RANGE_2 == CurrentRange)
	{
		Kp = DataTable[REG_REGULATOR_RANGE2_Kp];
		Ki = DataTable[REG_REGULATOR_RANGE2_Ki];
		KiTune = (CurrentMax - CurrentTarget) * DataTable[REG_REGULATOR_TF_Ki_RANG2];
	}
	else
	{
		Kp = DataTable[REG_REGULATOR_RANGE0_Kp + CurrentRange * 2];
		Ki = DataTable[REG_REGULATOR_RANGE0_Ki + CurrentRange * 2];
		KiTune = (CurrentMax - CurrentTarget) * DataTable[REG_REGULATOR_TF_Ki_RANG0 + CurrentRange];
	}

	PulseCounter = 0;
	DACLimitValue =
			(DAC_MAX_VAL > DataTable[REG_DAC_OUTPUT_LIMIT_VALUE]) ? DataTable[REG_DAC_OUTPUT_LIMIT_VALUE] : DAC_MAX_VAL;
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALLOWED_ERR];
	FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];
	DisableRegulator = DataTable[REG_DBG_DISABLE_REGLTR];
}
//-----------------------------------------------
