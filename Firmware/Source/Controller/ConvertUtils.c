// Includes
//
#include "ConvertUtils.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "Global.h"

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
ConvertParams AdcToCurrentParams[CURRENT_RANGE_QUANTITY];
ConvertParams CurrentToDacParams[CURRENT_RANGE_QUANTITY];

// Functions prototypes
float CU_ADCtoX(Int16U Data, ConvertParams* Coefficients);

// Functions
//
float CU_ItoDAC(float Current, Int16U CurrentRange)
{
	// Пересчет амплитуды тока в расчете на одну CurrentBoard
	Current = Current / DataTable[REG_CURBOARD_QUANTITY];
	return (Current + CurrentToDacParams[CurrentRange].B) * CurrentToDacParams[CurrentRange].K;
}
//-----------------------------

float CU_ADCtoX(Int16U Data, ConvertParams* Coefficients)
{
	return (Data * Coefficients->K + Coefficients->B);
}
//-----------------------------

float CU_ADCtoI(Int16U Data, Int16U CurrentRange)
{
	float Uadc, Current;

	Uadc = CU_ADCtoX(Data, &AdcToCurrentParams[CurrentRange]);
	Current = Uadc / AdcToCurrentParams[CurrentRange].Kamp / DataTable[REG_SHUNT_RESISTANCE] * 1000;

	return (Current * Current * AdcToCurrentParams[CurrentRange].P2 + Current * AdcToCurrentParams[CurrentRange].P1 + AdcToCurrentParams[CurrentRange].P0);
}
//-----------------------------

float CU_ADCtoV(Int16U Data)
{
	return CU_ADCtoX(Data, &AdcToVoltageParams);
}
//-----------------------------

void CU_LoadConvertParams()
{
	// Параметры преобразования значения АЦП в напряжение
	AdcToVoltageParams.K = (float)DataTable[REG_ADC_VOLTAGE_K] / 1e6;
	AdcToVoltageParams.B = (Int16S)DataTable[REG_ADC_VOLTAGE_B];

	// Параметры преобразования значения АЦП в ток и тока в ЦАП
	for(int i = 0; i < CURRENT_RANGE_QUANTITY; i++)
	{
		AdcToCurrentParams[i].P2 = (float)((Int16S)DataTable[REG_ADC_I_RANGE0_P2 + i * 6]) / 1e6;
		AdcToCurrentParams[i].P1 = (float)DataTable[REG_ADC_I_RANGE0_P1 + i * 6] / 1000;
		AdcToCurrentParams[i].P0 = (float)((Int16S)DataTable[REG_ADC_I_RANGE0_P0 + i * 6]) / 10;
		AdcToCurrentParams[i].K = (float)DataTable[REG_ADC_I_RANGE0_N + i * 6] / DataTable[REG_ADC_I_RANGE0_D + i * 6];
		AdcToCurrentParams[i].B = (Int16S)DataTable[REG_ADC_I_RANGE0_B + i * 6];
		AdcToCurrentParams[i].Kamp = (float)DataTable[REG_K_AMP_RANGE0 + i] / 100;

		CurrentToDacParams[i].K = (float)DataTable[REG_I_TO_DAC_RANGE0_K + i * 2] / 1000;
		CurrentToDacParams[i].B = (Int16S)DataTable[REG_I_TO_DAC_RANGE0_B + i * 2];
	}
}
//-----------------------------

