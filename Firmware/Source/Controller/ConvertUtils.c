// Includes
//
#include "ConvertUtils.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "Global.h"
#include "Constraints.h"

// Structs
typedef struct __ConvertParams
{
	float P2;
	float P1;
	float P0;
	float K;
	float B;
	float Kamp;
} ConvertParams;

// Variables
ConvertParams AdcToVoltageParams;
ConvertParams AdcToCurrentParams;
ConvertParams CurrentToDacParams;

// Functions prototypes
float CU_ADCtoX(float Data, ConvertParams* Coefficients);

// Functions
float CU_ItoDAC(float Current)
{
	float K, B;

	K = CurrentToDacParams.K;
	B = CurrentToDacParams.B;

	return Current * K + B;
}
//-----------------------------

float CU_ItoIcorrect(float Current)
{
	return Current * Current * CurrentToDacParams.P2 + Current * CurrentToDacParams.P1 + CurrentToDacParams.P0;
}
//-----------------------------

float CU_ADCtoX(float Data, ConvertParams* Coefficients)
{
	float Uadc;
	switch((Int16U)DataTable[REG_PCB_VERSION])
	{
		case PCB_VERSION_10:
			Uadc = Data * ADC_REF_VOLTAGE_PCB10 / ADC_RESOLUTION;
			return (Uadc * Coefficients->K + Coefficients->B);

		case PCB_VERSION_11:
			{
				float Uref;
				Uref = (DataTable[REG_REF_VOLTAGE_VARIABLE] == 0) ? ADC_REF_VOLTAGE_PCB11 : DataTable[REG_REF_VOLTAGE_VARIABLE];
				Uadc = Data * Uref / ADC_RESOLUTION;
				return (Uadc * Coefficients->K + Coefficients->B);
			}
			break;

		default:
			return 0;
	}
}
//-----------------------------

float CU_ADCtoI(float Data)
{
	float Uadc, Current;

	Uadc = CU_ADCtoX(Data, &AdcToCurrentParams);
	if (DataTable[REG_PCB_VERSION] == PCB_VERSION_10)
	{
		Current = Uadc / AdcToCurrentParams.Kamp / DataTable[REG_SHUNT_RESISTANCE] * 1000;
	}
	else
	{
		Current = Uadc / DataTable[REG_SHUNT_RESISTANCE] * 1000;
	}
	return (Current * Current * AdcToCurrentParams.P2 + Current * AdcToCurrentParams.P1 + AdcToCurrentParams.P0);
}
//-----------------------------

float CU_ADCtoV(float Data)
{
	return CU_ADCtoX(Data, &AdcToVoltageParams);
}
//-----------------------------

void CU_LoadConvertParams()
{
	// Параметры преобразования значения АЦП в напряжение
	AdcToVoltageParams.K = DataTable[REG_ADC_VOLTAGE_K];
	AdcToVoltageParams.B = DataTable[REG_ADC_VOLTAGE_B];

	// Параметры преобразования значения АЦП в ток и тока в ЦАП
	switch(CONTROL_GetCurrentRange())
	{
		case CurrentRange0:
			AdcToCurrentParams.P2 = DataTable[REG_ADC_I_RANGE0_1_P2];
			AdcToCurrentParams.P1 = DataTable[REG_ADC_I_RANGE0_1_P1];
			AdcToCurrentParams.P0 = DataTable[REG_ADC_I_RANGE0_1_P0];
			AdcToCurrentParams.K = DataTable[REG_ADC_I_RANGE0_1_K];
			AdcToCurrentParams.B = DataTable[REG_ADC_I_RANGE0_1_B];
			AdcToCurrentParams.Kamp = DataTable[REG_K_AMP_RANGE0];

			CurrentToDacParams.P2 = DataTable[REG_DAC_I_RANGE0_P2];
			CurrentToDacParams.P1 = DataTable[REG_DAC_I_RANGE0_P1];
			CurrentToDacParams.P0 = DataTable[REG_DAC_I_RANGE0_P0];
			CurrentToDacParams.K = DataTable[REG_DAC_I_RANGE0_K];
			CurrentToDacParams.B = DataTable[REG_DAC_I_RANGE0_B];
			break;

		case CurrentRange1:
			AdcToCurrentParams.P2 = DataTable[REG_ADC_I_RANGE1_P2];
			AdcToCurrentParams.P1 = DataTable[REG_ADC_I_RANGE1_P1];
			AdcToCurrentParams.P0 = DataTable[REG_ADC_I_RANGE1_P0];
			AdcToCurrentParams.K = DataTable[REG_ADC_I_RANGE1_K];
			AdcToCurrentParams.B = DataTable[REG_ADC_I_RANGE1_B];
			AdcToCurrentParams.Kamp = DataTable[REG_K_AMP_RANGE1];

			CurrentToDacParams.P2 = DataTable[REG_DAC_I_RANGE1_P2];
			CurrentToDacParams.P1 = DataTable[REG_DAC_I_RANGE1_P1];
			CurrentToDacParams.P0 = DataTable[REG_DAC_I_RANGE1_P0];
			CurrentToDacParams.K = DataTable[REG_DAC_I_RANGE1_K];
			CurrentToDacParams.B = DataTable[REG_DAC_I_RANGE1_B];
			break;

		case CurrentRange2:
			AdcToCurrentParams.P2 = DataTable[REG_ADC_I_RANGE1_P2];
			AdcToCurrentParams.P1 = DataTable[REG_ADC_I_RANGE1_P1];
			AdcToCurrentParams.P0 = DataTable[REG_ADC_I_RANGE1_P0];
			AdcToCurrentParams.K = DataTable[REG_ADC_I_RANGE1_K];
			AdcToCurrentParams.B = DataTable[REG_ADC_I_RANGE1_B];
			AdcToCurrentParams.Kamp = DataTable[REG_K_AMP_RANGE1];

			CurrentToDacParams.P2 = DataTable[REG_DAC_I_RANGE2_P2];
			CurrentToDacParams.P1 = DataTable[REG_DAC_I_RANGE2_P1];
			CurrentToDacParams.P0 = DataTable[REG_DAC_I_RANGE2_P0];
			CurrentToDacParams.K = DataTable[REG_DAC_I_RANGE2_K];
			CurrentToDacParams.B = DataTable[REG_DAC_I_RANGE2_B];
			break;
	}
}
//-----------------------------

