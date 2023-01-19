#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
#include "DataTable.h"
#include "Global.h"

//Definitions
#define NO		0
#define YES		1
//
#define I_RANGE0_THRESHOLD_MIN		0			// A * 10
#define I_RANGE0_THRESHOLD_MAX		INT16U_MAX	// A * 10
#define I_RANGE0_THRESHOLD_DEF		10000		// A * 10
//
#define I_RANGE1_THRESHOLD_MIN		0			// A * 10
#define I_RANGE1_THRESHOLD_MAX		INT16U_MAX	// A * 10
#define I_RANGE1_THRESHOLD_DEF		65000		// A * 10
//
#define V_BAT_THRESHOLD_MIN			100			// В * 10
#define V_BAT_THRESHOLD_MAX			500			// В * 10
#define V_BAT_THRESHOLD_DEF			450			// В * 10
//
#define DAC_OUTPUT_LIM_MIN			0
#define DAC_OUTPUT_LIM_MAX			4095
#define DAC_OUTPUT_LIM_DEF			3500
//
#define DAC_OFFSET_MIN				0
#define DAC_OFFSET_MAX				4095
#define DAC_OFFSET_DEF				1300
//
#define FULL_CHARGE_TIMEOUT_MIN		30000		// мс
#define FULL_CHARGE_TIMEOUT_MAX		60000		// мс
#define FULL_CHARGE_TIMEOUT_DEF		50000		// мс
//
#define RECHARGE_TIMEOUT_MIN		0			// мс
#define RECHARGE_TIMEOUT_MAX		40000		// мс
#define RECHARGE_TIMEOUT_DEF		20000		// мс
//
#define SHUNT_RESISTANCE_MIN		0			// мкОм
#define SHUNT_RESISTANCE_MAX		5000		// мкОм
#define SHUNT_RESISTANCE_DEF		250			// мкОм
//
#define AFTER_PULSE_PAUSE_MIN		0			// мс
#define AFTER_PULSE_PAUSE_MAX		40000		// мс
#define AFTER_PULSE_PAUSE_DEF		15000		// мс
//
#define I_PER_CURBOARD_MIN			500			// A * 10
#define I_PER_CURBOARD_MAX			15000		// A * 10
#define I_PER_CURBOARD_DEF			10833		// A * 10
//
#define CURBOARD_QUANTITY_MIN		1
#define CURBOARD_QUANTITY_MAX		6
#define CURBOARD_QUANTITY_DEF		6
//
#define COEF_P2_MIN					0
#define COEF_P2_MAX					INT16U_MAX
#define COEF_P2_DEF					0
//
#define COEF_K_MIN					1
#define COEF_K_MAX					INT16U_MAX
#define COEF_K_DEF					1000
//
#define OFFSET_MIN					0
#define OFFSET_MAX					INT16U_MAX
#define OFFSET_DEF					0
//
#define REGULATOR_KP_MIN			0
#define REGULATOR_KP_MAX			INT16U_MAX
#define REGULATOR_KP_DEF			1000
//
#define REGULATOR_KI_MIN			0
#define REGULATOR_KI_MAX			INT16U_MAX
#define REGULATOR_KI_DEF			0
//
#define CURRENT_SETPOINT_MIN		1000		// (А * 10)
#define CURRENT_SETPOINT_MAX		65000		// (А * 10)
//
#define FAN_PERIOD_MIN				0			// сек
#define FAN_PERIOD_MAX				600			// сек
#define FAN_PERIOD_DEF				300			// сек
//
#define FAN_TIME_MIN				0			// сек
#define FAN_TIME_MAX				600			// сек
#define FAN_TIME_DEF				60			// сек
//
#define V_BAT_K_MIN					0
#define V_BAT_K_MAX					INT16U_MAX
#define V_BAT_K_DEF					13400
//
#define CONF_STATE_TIMEOUT_MIN		1000		// мс
#define CONF_STATE_TIMEOUT_MAX		10000		// мс
#define CONF_STATE_TIMEOUT_DEF		5000		// мс

// Types
typedef struct __TableItemConstraint
{
	float Min;
	float Max;
	float Default;
} TableItemConstraint;

// Variables
extern const TableItemConstraint Constraint[];

#endif // __CONSTRAINTS_H
