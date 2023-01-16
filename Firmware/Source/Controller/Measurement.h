#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

// Includes
#include "SysConfig.h"
#include "ZwBase.h"
#include "ConvertUtils.h"
#include "Regulator.h"

// Definitions
#define PULSE_ARR_MAX_LENGTH					300				// Количество точек оцифровки
//

// Variables
extern Int16U MEASURE_ADC_CurrentRaw[ADC_DMA_BUFF_SIZE];
extern Int16U MEASURE_ADC_BatteryVoltageRaw[ADC_DMA_BUFF_SIZE];

// Functions
float MEASURE_SingleSampleBatteryVoltage();
void MEASURE_SampleParams(volatile RegulatorParamsStruct* Regulator);
void MEASURE_DMABufferClear();
void MEASURE_SetCurrentRange(volatile RegulatorParamsStruct* Regulator);

#endif /* MEASUREMENT_H_ */
