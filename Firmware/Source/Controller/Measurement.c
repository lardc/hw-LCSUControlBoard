// Includes
#include "Measurement.h"
#include "Board.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "Global.h"
#include "Interrupts.h"

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
	INT_VBatReady = false;
	ADC_SamplingStart(ADC1);
	while(!INT_VBatReady){}

	return CU_ADCtoV(MEASURE_DMAExtractVolatge());
}
//-----------------------------------------------

void MEASURE_SampleParams(volatile RegulatorParamsStruct* Regulator)
{
	Regulator->MeasuredCurrent = CU_ADCtoI(MEASURE_DMAExtractCurrent());
	Regulator->MeasuredBatteryVoltage = CU_ADCtoV(MEASURE_DMAExtractVolatge());
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
	return MEASURE_DMAExtractX(&MEASURE_ADC_CurrentRaw[1], ADC_DMA_BUFF_SIZE - 1);
}
//-----------------------------------------------

float MEASURE_DMAExtractVolatge()
{
	return MEASURE_DMAExtractX(&MEASURE_ADC_BatteryVoltageRaw[1], ADC_DMA_BUFF_SIZE - 1);
}
//-----------------------------------------------

int MEASURE_SortCondition(const void *A, const void *B) {
    float a = *(float*)A, b = *(float*)B;
    if (a > b) return 1;
    if (a < b) return -1;
    return 0;
}
//-----------------------------------------------
