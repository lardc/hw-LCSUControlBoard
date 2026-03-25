#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

// Includes
#include "SysConfig.h"
#include "ZwBase.h"
#include "ConvertUtils.h"
#include "Regulator.h"

// Variables
extern Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];
extern Int16U MEASURE_ADC_BatteryVoltageRaw[ADC_DMA_BUFF_SIZE];

// Functions
float MEASURE_SingleSampleBatteryVoltage();
void MEASURE_SampleParams(volatile RegulatorParamsStruct* Regulator);
int MEASURE_SortCondition(const void *A, const void *B);

#endif /* MEASUREMENT_H_ */
