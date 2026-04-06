// Header
//
#include "Regulator.h"
#include "DataTable.h"
#include "LowLevel.h"
#include "ConvertUtils.h"
#include "Controller.h"
#include <math.h>
#include "Measurement.h"

// Definitions
//
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
Int16U REGULATOR_FlattopLastIndex;
static Int16U DACLimitValue, FollowingErrorCounterMax, PulseLengthTicks, PulseCounter;
static float RegulatorAlowedError, Kp, Ki, QiMax, PulseAmplitude, TrapezeRate;
static bool DisableRegulator, DisableFollowingError, DisableDACOutput, KiMute;
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
	static float Qi = 0, PrevSetpointCurrent = 0;
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

		PrevSetpointCurrent = 0;
		Qi = 0;
		FollowingErrorCounter = 0;
	}
	else
	{
		// Для ошибки берётся предыдущее задание
		RegulatorError = PrevSetpointCurrent - MeasuredCurrent;
		RegulatorRelativeError = (PrevSetpointCurrent == 0) ? 0 : (RegulatorError / PrevSetpointCurrent);
	}

	// Проверка Following Error
	if(!DisableFollowingError && !DisableRegulator)
	{
		if(fabsf(RegulatorRelativeError * 100) < RegulatorAlowedError)
			FollowingErrorCounter = 0;
		else
			FollowingErrorCounter++;

		if(FollowingErrorCounter >= FollowingErrorCounterMax)
		{
			REGULATOR_LoggingData(MeasuredCurrent, MeasuredBatteryVoltage, 0, RegulatorRelativeError, 0, 0);
			*Problem = PROBLEM_FOLLOWING_ERROR;
			return true;
		}
	}

	// Расчёт корректировок
	float Qp = RegulatorError * Kp;
	Qi += RegulatorError * (KiMute ? 0 : Ki);

	if(Qi > QiMax)
		Qi = QiMax;
	else if (Qi < -QiMax)
		Qi = -QiMax;

	// Задание
	float SetpointCurrent = REGULATOR_GetCurrent(PulseCounter);
	// Скорректированное значение
	float RegulatorOutput = SetpointCurrent + (DisableRegulator ? 0 : (Qp + Qi));

	// Пересчёт в ЦАП
	float ValueToDAC = CU_ItoDAC(RegulatorOutput);
	Int16U DACSetpoint = REGULATOR_DACApplyLimits(ValueToDAC, DACLimitValue);
	LL_WriteDAC(DisableDACOutput ? 0 : DACSetpoint);

	REGULATOR_LoggingData(MeasuredCurrent, MeasuredBatteryVoltage, RegulatorOutput,
			RegulatorRelativeError, SetpointCurrent, DACSetpoint);
	PulseCounter++;
	PrevSetpointCurrent = SetpointCurrent;

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

	// Условия досрочного завершения расчёта
	if(Tick == 0)
	{
		PrevCurrent = 0;
		return 0;
	}
	else if(Tick >= VALUES_x_SIZE)
	{
		PrevCurrent = 0;
		PState = PST_Break;
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
				TailDecay = LINEAR_FRAGMENT_AMPLITUDE / (VALUES_x_SIZE - Tick);
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
				KiMute = false;
				PState = PST_TrapezeFlattop;
			}
			break;

		case PST_TrapezeFlattop:
			Current = PrevCurrent;
			FlattopCounter++;
			if(FlattopCounter >= PulseLengthTicks)
			{
				REGULATOR_FlattopLastIndex = Tick;
				PState = PST_TrapezeFall;
			}
			break;

		case PST_TrapezeFall:
			Current = PrevCurrent - TrapezeRate;
			break;

		default:
			break;
	}

	// Условие окончания формирования
	if(Current < 0 || Tick >= VALUES_x_SIZE)
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
	if(CONTROL_Values_Counter >= VALUES_x_SIZE)
		return;

	CONTROL_ValuesCurrent[CONTROL_Values_Counter] = MeasuredCurrent;
	CONTROL_ValuesBatteryVoltage[CONTROL_Values_Counter] = MeasuredBatteryVoltage;
	CONTROL_RegulatorOutput[CONTROL_Values_Counter] = RegulatorOutput;
	CONTROL_RegulatorErr[CONTROL_Values_Counter] = RegulatorError * 100;
	CONTROL_CurentTable[CONTROL_Values_Counter] = Setpoint;
	CONTROL_DACRawData[CONTROL_Values_Counter] = DACValue;

	CONTROL_Values_Counter++;
}
//-----------------------------------------------

void REGULATOR_CacheVariables()
{
	PulseAmplitude = DataTable[REG_CURRENT_PULSE_VALUE];

	// Кеширование коэффициентов регулятора
	Int16U CurrentRange = CONTROL_GetCurrentRange();
	if(CurrentRange2 == CurrentRange)
	{
		Kp = DataTable[REG_REGULATOR_RANGE2_Kp];
		Ki = DataTable[REG_REGULATOR_RANGE2_Ki];
	}
	else
	{
		Kp = DataTable[REG_REGULATOR_RANGE0_Kp + CurrentRange * 2];
		Ki = DataTable[REG_REGULATOR_RANGE0_Ki + CurrentRange * 2];
	}
	QiMax = DataTable[REG_REGULATOR_QI_MAX];

	REGULATOR_FlattopLastIndex = 0;
	PulseCounter = 0;
	PShape = DataTable[REG_PULSE_SHAPE];
	KiMute = false;
	switch(PShape)
	{
		case PSH_Sine:
			PState = PST_Sine;
			break;
		case PSH_ModSine:
			PState = PST_ModSine;
			break;
		case PSH_Trapeze:
			KiMute = true;
			PState = PST_TrapezeRise;
			break;
		default:
			PState = PST_Break;
			break;
	}
	PulseLengthTicks = ((PShape == PSH_Trapeze) ? (DataTable[REG_TRAPEZE_FLATTOP] * 1000) : SINE_PULSE_DURATION)
			/ TIMER15_uS;
	TrapezeRate = DataTable[REG_TRAPEZE_CURRENT_RATE] * TIMER15_uS;
	DACLimitValue =
			(DAC_MAX_VAL > DataTable[REG_DAC_OUTPUT_LIMIT_VALUE]) ? DataTable[REG_DAC_OUTPUT_LIMIT_VALUE] : DAC_MAX_VAL;
	RegulatorAlowedError = DataTable[REG_REGULATOR_ALLOWED_ERR];
	FollowingErrorCounterMax = DataTable[REG_FOLLOWING_ERR_CNT];

	DisableFollowingError = DataTable[REG_FOLLOWING_ERR_MUTE];
	DisableRegulator = DataTable[REG_DBG_DISABLE_REGLTR];
	DisableDACOutput = DataTable[REG_DBG_DISABLE_DAC_OUTPUT];
}
//-----------------------------------------------
