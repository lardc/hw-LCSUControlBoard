#ifndef CONVERTUTILS_H_
#define CONVERTUTILS_H_

// Includes
#include "ZwBase.h"

// Functions
float CU_ItoDAC(float Current, Int16U CurrentRange);
float CU_ADCtoI(Int16U Data, Int16U CurrentRange);
float CU_ItoIcorrect(float Current, Int16U CurrentRange);
float CU_ADCtoV(Int16U Data);
void CU_LoadConvertParams();

#endif /* CONVERTUTILS_H_ */
