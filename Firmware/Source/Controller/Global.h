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
#define LINEAR_FRAGMENT_DURATION				20000	// Длительность линейной части мод. полусинуса (мкс)
#define LINEAR_FRAGMENT_AMPLITUDE				30.0f	// Амплитуда тока для начала формирования хвоста (А)
//
#define DAC_MAX_VAL								0x0FFF	// Максимальное значение ЦАП
#define	CURRENT_RANGES							3		// Количество диапазонов измерения тока
#define CURRENT_RANGE_0							0
#define CURRENT_RANGE_1							1
#define CURRENT_RANGE_2							2
//
#define ADC_REF_VOLTAGE_PCB10					3300.0f	// Опорное напряжение (в мВ)
#define ADC_REF_VOLTAGE_PCB11					3000.0f	// Опорное напряжение (в мВ)
#define ADC_RESOLUTION							4095	// Разрешение АЦП
//
#define SINE_SHAPE								0
#define MOD_SINE_SHAPE							1
#define TRAPEZE_SHAPE							2
//
#define SIZE_INDEX								10 // Размер массива для определения максимального тока около значения с максимальным индексом ЦАПа (в тактах регулятора)
#define SIZE_WINDOW								6 // Размер массива после отсечения выбросов (в тактах регулятора)
#define INDENT_ZONE                             2 // Точки для отсечения выбросов (в тактах регулятора)
//

// Размер массивов EP
#define VALUES_x_SIZE	((SINE_PULSE_DURATION + LINEAR_FRAGMENT_DURATION) / TIMER15_uS)
#define VALUES_EXT_INFO_SIZE						300

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)
#define TIME_FAULT_EXT_LED_BLINK				250		// Период мигания внешней индикации в состоянии Fault (в мс)
#define EXT_LAMP_ON_STATE_TIME					500		// Время зажигания внешнего индикатора во время импульса (в мс)

#endif //  __GLOBAL_H
