// Includes
#include "Measurement.h"
#include "Board.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "Global.h"
#include "Interrupts.h"
#include "ConvertUtils.h"
#include "Delay.h"

// Variables
//
Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];
Int16U MEASURE_ADC_BatteryVoltageRaw[ADC_DMA_BUFF_SIZE];

// Functions prototypes
float MEASURE_DMAExtractX(Int16U* InputArray, Int16U ArraySize);
float MEASURE_DMAExtractCurrent();
float MEASURE_DMAExtractVolatge();

// Functions
//
float MEASURE_SingleSampleBatteryVoltage()
{
	Int16U SafeCounter = 0;
	const Int16U SafeCounterMax = 100;

	// Программный запуск измерения
	INT_VBatReady = false;
	ADC_SamplingStart(ADC1);
	while(!INT_VBatReady && SafeCounter < SafeCounterMax)
	{
		SafeCounter++;
		DELAY_US(1);
	}
	INT_VBatReady = false;

	return (SafeCounter >= SafeCounterMax) ? 0 : CU_ADCtoV(MEASURE_DMAExtractVolatge());
}
//-----------------------------------------------

void MEASURE_SampleParams(float *Current, float *BatteryVoltage)
{
	*Current = CU_ADCtoI(MEASURE_DMAExtractCurrent());
	*BatteryVoltage = CU_ADCtoV(MEASURE_DMAExtractVolatge());
}
//-----------------------------------------------

float MEASURE_DMAExtractX(Int16U* InputArray, Int16U ArraySize)
{
	float AverageData = 0;

	for(int i = 0; i < ArraySize; i++)
		AverageData += *(InputArray + i);

	return (AverageData / ArraySize);
}
//-----------------------------------------------

float MEASURE_DMAExtractCurrent()
{
	return MEASURE_DMAExtractX(MEASURE_ADC_CurrentRaw, ADC_DMA_BUFF_SIZE);
}
//-----------------------------------------------

float MEASURE_DMAExtractVolatge()
{
	return MEASURE_DMAExtractX(MEASURE_ADC_BatteryVoltageRaw, ADC_DMA_BUFF_SIZE);
}
//-----------------------------------------------
