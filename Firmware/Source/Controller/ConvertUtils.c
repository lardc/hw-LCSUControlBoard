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
}ConvertParams;

// Variables
ConvertParams AdcToVoltageParams;
ConvertParams AdcToCurrentParams[CURRENT_RANGES];
ConvertParams CurrentToDacParams[CURRENT_RANGES];

// Functions prototypes
float CU_ADCtoX(float Data, ConvertParams* Coefficients);

// Functions
//
float CU_ItoDAC(float Current, Int16U CurrentRange)
{
	float K, B;

	K = CurrentToDacParams[CurrentRange].K;
	B = CurrentToDacParams[CurrentRange].B;

	// Пересчет амплитуды тока в расчете на одну CurrentBoard
	if(CurrentRange)
		Current = Current / DataTable[REG_CURBOARDS];

	return Current * K + B;
}
//-----------------------------
float CU_ItoIcorrect(float Current, Int16U CurrentRange)
{
	return Current * Current * CurrentToDacParams[CurrentRange].P2 + Current * CurrentToDacParams[CurrentRange].P1 + CurrentToDacParams[CurrentRange].P0;
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
				Uref = DataTable[REG_REF_VOLTAGE_VARIABLE] == 0 ? ADC_REF_VOLTAGE_PCB11 : DataTable[REG_REF_VOLTAGE_VARIABLE];
				Uadc = Data * Uref / ADC_RESOLUTION;
				return (Uadc * Coefficients->K + Coefficients->B);
			}
			break;

		default:
			return 0;
	}
}
//-----------------------------

float CU_ADCtoI(float Data, Int16U CurrentRange)
{
	float Uadc, Current;

	Uadc = CU_ADCtoX(Data, &AdcToCurrentParams[CurrentRange]);
	if (DataTable[REG_PCB_VERSION] == PCB_VERSION_10)
	{
		Current = Uadc / AdcToCurrentParams[CurrentRange].Kamp / DataTable[REG_SHUNT_RESISTANCE] * 1000;
	}
	else
	{
		Current = Uadc / DataTable[REG_SHUNT_RESISTANCE] * 1000;
	}
	return (Current * Current * AdcToCurrentParams[CurrentRange].P2 + Current * AdcToCurrentParams[CurrentRange].P1 + AdcToCurrentParams[CurrentRange].P0);
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
	for(int i = 0; i < CURRENT_RANGES; i++)
	{
		switch(i)
		{
			case CURRENT_RANGE_0:
				AdcToCurrentParams[i].P2 = DataTable[REG_ADC_I_RANGE0_1_P2];
				AdcToCurrentParams[i].P1 = DataTable[REG_ADC_I_RANGE0_1_P1];
				AdcToCurrentParams[i].P0 = DataTable[REG_ADC_I_RANGE0_1_P0];
				AdcToCurrentParams[i].K = DataTable[REG_ADC_I_RANGE0_1_K];
				AdcToCurrentParams[i].B = DataTable[REG_ADC_I_RANGE0_1_B];
				AdcToCurrentParams[i].Kamp = DataTable[REG_K_AMP_RANGE0];

				CurrentToDacParams[i].P2 = DataTable[REG_DAC_I_RANGE0_P2];
				CurrentToDacParams[i].P1 = DataTable[REG_DAC_I_RANGE0_P1];
				CurrentToDacParams[i].P0 = DataTable[REG_DAC_I_RANGE0_P0];
				CurrentToDacParams[i].K = DataTable[REG_DAC_I_RANGE0_K];
				CurrentToDacParams[i].B = DataTable[REG_DAC_I_RANGE0_B];
				break;

			case CURRENT_RANGE_1:
				AdcToCurrentParams[i].P2 = DataTable[REG_ADC_I_RANGE0_1_P2];
				AdcToCurrentParams[i].P1 = DataTable[REG_ADC_I_RANGE0_1_P1];
				AdcToCurrentParams[i].P0 = DataTable[REG_ADC_I_RANGE0_1_P0];
				AdcToCurrentParams[i].K = DataTable[REG_ADC_I_RANGE0_1_K];
				AdcToCurrentParams[i].B = DataTable[REG_ADC_I_RANGE0_1_B];
				AdcToCurrentParams[i].Kamp = DataTable[REG_K_AMP_RANGE0];

				CurrentToDacParams[i].P2 = DataTable[REG_DAC_I_RANGE1_P2];
				CurrentToDacParams[i].P1 = DataTable[REG_DAC_I_RANGE1_P1];
				CurrentToDacParams[i].P0 = DataTable[REG_DAC_I_RANGE1_P0];
				CurrentToDacParams[i].K = DataTable[REG_DAC_I_RANGE1_K];
				CurrentToDacParams[i].B = DataTable[REG_DAC_I_RANGE1_B];
				break;

			case CURRENT_RANGE_2:
				AdcToCurrentParams[i].P2 = DataTable[REG_ADC_I_RANGE1_P2];
				AdcToCurrentParams[i].P1 = DataTable[REG_ADC_I_RANGE1_P1];
				AdcToCurrentParams[i].P0 = DataTable[REG_ADC_I_RANGE1_P0];
				AdcToCurrentParams[i].K = DataTable[REG_ADC_I_RANGE1_K];
				AdcToCurrentParams[i].B = DataTable[REG_ADC_I_RANGE1_B];
				AdcToCurrentParams[i].Kamp = DataTable[REG_K_AMP_RANGE1];

				CurrentToDacParams[i].P2 = DataTable[REG_DAC_I_RANGE2_P2];
				CurrentToDacParams[i].P1 = DataTable[REG_DAC_I_RANGE2_P1];
				CurrentToDacParams[i].P0 = DataTable[REG_DAC_I_RANGE2_P0];
				CurrentToDacParams[i].K = DataTable[REG_DAC_I_RANGE2_K];
				CurrentToDacParams[i].B = DataTable[REG_DAC_I_RANGE2_B];
				break;
		}
	}
}
//-----------------------------

