// Includes
#include "Measurement.h"
#include "Board.h"
#include "LowLevel.h"
#include "DataTable.h"
#include "Global.h"
#include "Interrupts.h"
#include "ConvertUtils.h"
#include "Delay.h"
#include "stdlib.h"
#include <math.h>

// Definitions
typedef struct __FindMaxEntity
{
	float Value;
	Int16U Index;
} FindMaxEntity;

// Variables
Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];
Int16U MEASURE_ADC_BatteryVoltageRaw[ADC_DMA_BUFF_SIZE];
static FindMaxEntity FindMax[VALUES_x_SIZE];

// Functions prototypes
float MEASURE_DMAExtractX(Int16U* InputArray, Int16U ArraySize);
float MEASURE_DMAExtractCurrent();
float MEASURE_DMAExtractVolatge();
int MEASURE_SortCondition(const void *A, const void *B);

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

void MEASURE_FindMax(float *CurrentInputArray, float *DACInputArray, Int16U Elements, float *AvgDAC, float *AvgCurrent)
{
	// Копирование значений
	for(Int16U i = 0; i < Elements && i < VALUES_x_SIZE; i++)
	{
		FindMax[i].Value = CurrentInputArray[i];
		FindMax[i].Index = i;
	}

	// Cортировка по возрастанию
	qsort((void *)FindMax, Elements, sizeof(FindMaxEntity), MEASURE_SortCondition);

	// Поиск среднего индекса максимума
	float AvgIndex = 0;
	Int16U MaxIndex = 0;
	for(Int16U i = 0; i < RESULT_SIN_MAX_POINTS; i++)
		AvgIndex += FindMax[i].Index;
	MaxIndex = roundf(AvgIndex / RESULT_SIN_MAX_POINTS);

	// Взятие средних значений тока и ЦАП вокруг среднего максимального индекса
	Int16U cnt = 0;
	*AvgDAC = 0, *AvgCurrent = 0;
	for(Int16U i = MaxIndex - RESULT_SIN_EXTRA_POINTS; i <= MaxIndex + RESULT_SIN_EXTRA_POINTS; i++)
	{
		*AvgDAC += DACInputArray[i];
		*AvgCurrent += CurrentInputArray[i];
		cnt++;
	}
	*AvgDAC /= cnt;
	*AvgCurrent /= cnt;
}
//-----------------------------------------------

// Сортировка по убыванию по значениям
int MEASURE_SortCondition(const void *A, const void *B)
{
	FindMaxEntity a = *(FindMaxEntity*)A, b = *(FindMaxEntity*)B;

	if(a.Value > b.Value)
		return -1;

	if(a.Value < b.Value)
		return 1;

	return 0;
}
//-----------------------------------------------
