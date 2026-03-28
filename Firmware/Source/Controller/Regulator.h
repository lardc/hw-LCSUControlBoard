#ifndef REGULATOR_H_
#define REGULATOR_H_

// Includes
//
#include "stdinc.h"
#include "Global.h"
#include "SysConfig.h"

// Definitions
//
typedef enum __PulseShape
{
	PSH_Sine,
	PSH_ModSine,
	PSH_Trapeze
} PulseShape;

// Functions
//
bool REGULATOR_Process(pInt16U Problem);
void REGULATOR_CashVariables();

#endif /* REGULATOR_H_ */
