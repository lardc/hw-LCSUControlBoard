#ifndef CONVERTUTILS_H_
#define CONVERTUTILS_H_

// Includes
#include "ZwBase.h"

// Definitions
typedef enum __CurrentRanges
{
	CurrentRange0,
	CurrentRange1,
	CurrentRange2
} CurrentRanges;

// Functions
float CU_ItoDAC(float Current);
float CU_ADCtoI(float Data);
float CU_ADCtoV(float Data);
void CU_LoadConvertParams();

#endif /* CONVERTUTILS_H_ */
