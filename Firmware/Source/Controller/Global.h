#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "SysConfig.h"
#include "stdinc.h"

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS						1000	// Таймаут интерфейса SCCI (в мс)
#define	BCCIM_TIMEOUT_TICKS						50		// Таймаут протоколоа мастер BCCI (в мс)
//
#define EP_WRITE_COUNT							0		// Количество массивов для записи
#define EP_COUNT								0		// Количество массивов для чтения типа Int
#define FEP_COUNT								7		// Количество массивов для чтения типа float
#define ENABLE_LOCKING							FALSE	// Защита NV регистров паролем
//
#define SINE_PULSE_DURATION						10000	// Длительность импульса тока (мкс)
#define MAX_REGULATOR_DURATION					30000	// Максимальная длительность работы регулятора (мкс)
														// включая время формирования хвоста модифицированного синуса
#define LINEAR_FRAGMENT_AMPLITUDE				30.0f	// Амплитуда тока для начала формирования хвоста (А)
//
#define DAC_MAX_VAL								0x0FFF	// Максимальное значение ЦАП
#define ADC_REF_VOLTAGE_PCB10					3300.0f	// Опорное напряжение (в мВ)
#define ADC_REF_VOLTAGE_PCB11					3000.0f	// Опорное напряжение (в мВ)
#define ADC_RESOLUTION							4095	// Разрешение АЦП
//
#define RESULT_SIN_EXTRA_POINTS					1		// Число добавленных точек для синуса
#define RESULT_TRAPEZE_POINTS					3		// Число последних точек для трапеции

// Размер массивов EP
#define VALUES_x_SIZE							(MAX_REGULATOR_DURATION / TIMER15_uS)
#define VALUES_EXT_INFO_SIZE					300

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)
#define TIME_FAULT_EXT_LED_BLINK				250		// Период мигания внешней индикации в состоянии Fault (в мс)
#define EXT_LAMP_ON_STATE_TIME					500		// Время зажигания внешнего индикатора во время импульса (в мс)

#endif //  __GLOBAL_H
