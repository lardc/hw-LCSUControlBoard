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
#define CURRENT_THRESHOLD_MIN		0			// A
#define CURRENT_THRESHOLD_MAX		1000		// A
#define CURRENT_THRESHOLD_DEF		1000		// A
//
#define V_BAT_THRESHOLD_MIN			10			// В
#define V_BAT_THRESHOLD_MAX			50			// В
#define V_BAT_THRESHOLD_DEF			45			// В
//
#define DAC_OUTPUT_LIM_MIN			0
#define DAC_OUTPUT_LIM_MAX			4095
#define DAC_OUTPUT_LIM_DEF			3500
//
#define DAC_OFFSET_MIN				0
#define DAC_OFFSET_MAX				4095
#define DAC_OFFSET_DEF				250
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
#define I_PER_CURBOARD_MIN			50			// A
#define I_PER_CURBOARD_MAX			1500		// A
#define I_PER_CURBOARD_DEF			1084		// A
//
#define CURBOARDS_MIN				1
#define CURBOARDS_MAX				6
#define CURBOARDS_DEF				6
//
#define COEF_P2_MIN					-INT16S_MAX
#define COEF_P2_MAX					INT16S_MAX
#define COEF_P2_DEF					0
//
#define COEF_P1_MIN					-INT16S_MAX
#define COEF_P1_MAX					INT16S_MAX
#define COEF_P1_DEF					1
//
#define COEF_P0_MIN					-INT16S_MAX
#define COEF_P0_MAX					INT16S_MAX
#define COEF_P0_DEF					0
//
#define COEF_K_MIN					-INT16S_MAX
#define COEF_K_MAX					INT16S_MAX
#define COEF_K_DEF					1
//
#define OFFSET_MIN					-INT16S_MAX
#define OFFSET_MAX					INT16S_MAX
#define OFFSET_DEF					0
//
#define REGULATOR_KP_MIN			-INT16S_MAX
#define REGULATOR_KP_MAX			INT16S_MAX
#define REGULATOR_KP_DEF			0
//
#define REGULATOR_KI_MIN			-INT16S_MAX
#define REGULATOR_KI_MAX			INT16S_MAX
#define REGULATOR_KI_DEF			0
//
#define CURRENT_SETPOINT_MIN		30			// (А)
#define CURRENT_SETPOINT_MAX		6500		// (А)
//
#define FAN_PERIOD_MIN				0			// сек
#define FAN_PERIOD_MAX				600			// сек
#define FAN_PERIOD_DEF				300			// сек
//
#define FAN_TIME_MIN				0			// сек
#define FAN_TIME_MAX				600			// сек
#define FAN_TIME_DEF				60			// сек
//
#define V_BAT_K_MIN					-INT16S_MAX
#define V_BAT_K_MAX					INT16S_MAX
#define V_BAT_K_DEF					1
//
#define CONF_STATE_TIMEOUT_MIN		1000		// мс
#define CONF_STATE_TIMEOUT_MAX		10000		// мс
#define CONF_STATE_TIMEOUT_DEF		5000		// мс
//
#define TRAPEZE_CUR_RATE_MIN		1			// А/мкс
#define TRAPEZE_CUR_RATE_MAX		10			// A/мкс
#define TRAPEZE_CUR_RATE_DEF		3			// A/мкс
//
#define TRAPEZE_DURATION_MIN		1			// мс
#define TRAPEZE_DURATION_MAX		10			// мс
#define TRAPEZE_DURATION_DEF		10			// мс
//
#define REGULATOR_DELAY_MIN			0			// тактов
#define REGULATOR_DELAY_MAX			10			// тактов
#define REGULATOR_DELAY_DEF			4			// тактов

#define ALLOWED_ERR_MIN				0			// %
#define ALLOWED_ERR_MAX				15			// %
#define ALLOWED_ERR_DEF				10			// %

#define FOLLOWING_ERR_CNT_MIN		0
#define FOLLOWING_ERR_CNT_MAX		30
#define FOLLOWING_ERR_CNT_DEF		10

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
