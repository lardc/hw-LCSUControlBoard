// Header
//
#include "Regulator.h"
#include "DataTable.h"
#include "LowLevel.h"
#include "ConvertUtils.h"
#include "Controller.h"
#include "math.h"
#include "Measurement.h"

// Definitions
//
typedef enum __PulseShape
{
	PSH_Sine,
	PSH_ModSine,
	PSH_Trapeze
} PulseShape;

typedef enum __PulseState
{
	PST_Sine,
	PST_ModSine,
	PST_ModSineTail,
	PST_TrapezeRise,
	PST_TrapezeFlattop,
	PST_TrapezeFall,
	PST_Break
} PulseState;

// Variabls
//
static Int16U DACLimitValue, FollowingErrorCounterMax, PulseLengthTicks, PulseCounter;
static float RegulatorAlowedError, Kp, Ki, KiTune, QiMax, PulseAmplitude, TrapezeRate;
static bool DisableRegulator;
static PulseShape PShape;
static PulseState PState;

// Functions prototypes
//
float REGULATOR_GetCurrent(Int16U Tick);
void REGULATOR_LoggingData(float MeasuredCurrent, float MeasuredBatteryVoltage, float RegulatorOutput,
		float RegulatorError, float Setpoint, float DACValue);
Int16U REGULATOR_DACApplyLimits(float Value, Int16U LimitValue);

// Functions
//
bool REGULATOR_Process(pInt16U Problem)
{
	static float Qi = 0, PrevCurrent = 0;
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

		PrevCurrent = 0;
		Qi = 0;
		FollowingErrorCounter = 0;
	}
	else
	{
		// Для ошибки берётся предыдущее задание
		RegulatorError = PrevCurrent - MeasuredCurrent;
		RegulatorRelativeError = RegulatorError / PrevCurrent;
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
			*Problem = PROBLEM_FOLLOWING_ERROR;
			return true;
		}
	}

	// Расчёт корректировок
	float Qp = RegulatorError * Kp;
	Qi += RegulatorError * (Ki + KiTune);

	if(Qi > QiMax)
		Qi = QiMax;
	else if (Qi < -QiMax)
		Qi = -QiMax;

	// Задание
	float Current = REGULATOR_GetCurrent(PulseCounter);
	// Скорректированное значение
	float RegulatorOutput = Current + (DisableRegulator ? 0 : (Qp + Qi));

	// Пересчёт в ЦАП
	float ValueToDAC = CU_ItoDAC(RegulatorOutput);
	Int16U DACSetpoint = REGULATOR_DACApplyLimits(ValueToDAC, DACLimitValue);
	LL_WriteDAC(DACSetpoint);

	REGULATOR_LoggingData(MeasuredCurrent, MeasuredBatteryVoltage, RegulatorOutput,
			RegulatorError, 0, DACSetpoint);
	PulseCounter++;
	PrevCurrent = Current;

	return (PState == PST_Break);
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

float REGULATOR_GetCurrent(Int16U Tick)
{
	float Current = 0;
	static float PrevCurrent = 0, TailDecay = 0;
	static Int16U FlattopCounter = 0;

	if(Tick == 0)
	{
		PrevCurrent = 0;
		return 0;
	}

	switch(PState)
	{
		case PST_Sine:
		case PST_ModSine:
			Current = PulseAmplitude * sinf(M_PI * Tick / PulseLengthTicks);

			// Проверка условия выхода на хвост для модифицированного синуса
			if(PState == PST_ModSine && Tick > (PulseLengthTicks / 2) && Current < LINEAR_FRAGMENT_AMPLITUDE)
			{
				Current = LINEAR_FRAGMENT_AMPLITUDE;
				TailDecay = LINEAR_FRAGMENT_AMPLITUDE * (PulseLengthTicks - Tick);
				PState = PST_ModSineTail;
			}
			break;

		case PST_ModSineTail:
			Current = PrevCurrent - TailDecay;
			break;

		case PST_TrapezeRise:
			Current = PrevCurrent + TrapezeRate;
			if(Current >= PulseAmplitude)
			{
				Current = PulseAmplitude;
				FlattopCounter = 0;
				PState = PST_TrapezeFlattop;
			}
			break;

		case PST_TrapezeFlattop:
			FlattopCounter++;
			if(FlattopCounter >= PulseLengthTicks)
				PState = PST_TrapezeFall;
			break;

		case PST_TrapezeFall:
			Current = PrevCurrent - TrapezeRate;
			break;

		default:
			break;
	}

	if(Current < 0)
	{
		Current = 0;
		PState = PST_Break;
	}

	PrevCurrent = Current;
	return Current;
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
	PulseAmplitude = DataTable[REG_CURRENT_PULSE_VALUE];

	// Кеширование коэффициентов регулятора
	Int16U CurrentRange = CONTROL_GetCurrentRange();
	if(CURRENT_RANGE_2 == CurrentRange)
	{
		Kp = DataTable[REG_REGULATOR_RANGE2_Kp];
		Ki = DataTable[REG_REGULATOR_RANGE2_Ki];
		KiTune = (CurrentMax - PulseAmplitude) * DataTable[REG_REGULATOR_TF_Ki_RANG2];
	}
	else
	{
		Kp = DataTable[REG_REGULATOR_RANGE0_Kp + CurrentRange * 2];
		Ki = DataTable[REG_REGULATOR_RANGE0_Ki + CurrentRange * 2];
		KiTune = (CurrentMax - PulseAmplitude) * DataTable[REG_REGULATOR_TF_Ki_RANG0 + CurrentRange];
	}
	QiMax = DataTable[REG_REGULATOR_QI_MAX];

	PulseCounter = 0;
	PShape = DataTable[REG_PULSE_SHAPE];
	switch(PShape)
	{
		case PSH_Sine:
			PState = PST_Sine;
			break;
		case PSH_ModSine:
			PState = PST_ModSine;
			break;
		case PSH_Trapeze:
			PState = PST_TrapezeRise;
			break;
		default:
			PState = PST_Break;
			break;
	}
	PulseLengthTicks = ((PShape == PSH_Trapeze) ? (DataTable[REG_TRAPEZE_DURATION] * 1000) : SINE_PULSE_DURATION)
			/ TIMER15_uS;
	TrapezeRate = DataTable[REG_TRAPEZE_CURRENT_RATE] * TIMER15_uS;
	DACLimitValue =
			(DAC_MAX_VAL > DataTable[REG_DAC_OUTPUT_LIMIT_VALUE]) ? DataTable[REG_DAC_OUTPUT_LIMIT_VALUE] : DAC_MAX_VAL;
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALLOWED_ERR];
	FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];
	DisableRegulator = DataTable[REG_DBG_DISABLE_REGLTR];
}
//-----------------------------------------------
