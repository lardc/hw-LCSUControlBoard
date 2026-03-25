#ifndef CONVERTUTILS_H_
#define CONVERTUTILS_H_

// Includes
#include "ZwBase.h"

// Functions
float CU_ItoDAC(float Current);
float CU_ADCtoI(float Data);
float CU_ItoIcorrect(float Current);
float CU_ADCtoV(float Data);
void CU_LoadConvertParams();

#endif /* CONVERTUTILS_H_ */
