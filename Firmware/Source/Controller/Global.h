#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "SysConfig.h"

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS						1000	// Таймаут интерфейса SCCI (в мс)
#define	BCCIM_TIMEOUT_TICKS						50		// Таймаут протоколоа мастер BCCI (в мс)
//
#define EP_WRITE_COUNT							0		// Количество массивов для записи
#define EP_COUNT								0		// Количество массивов для чтения типа Int
#define FEP_COUNT								6		// Количество массивов для чтения типа float
#define ENABLE_LOCKING							FALSE	// Защита NV регистров паролем
//
#define SINE_PULSE_DURATION						10000	// Длительность импульса тока (мкс)
#define LINEAR_FRAGMENT_DURATION				5000	// Длительность линейной части мод. полусинуса (мкс)
#define LINEAR_FRAGMENT_AMPLITUDE				30.0f	// Амплитуда тока для начала формирования хвоста (А)
//
#define PI										3.1416f	// Значение числа Пи
#define DAC_MAX_VAL								0x0FFF	// Максимальное значение ЦАП
#define	CURRENT_RANGES							2		// Количество диапазонов измерения тока
#define CURRENT_RANGE_0							0
#define CURRENT_RANGE_1							1
//
#define SINE_SHAPE								0
#define MOD_SINE_SHAPE							1
#define TRAPEZE_SHAPE							2
//

// Размер массивов EP
#define VALUES_x_SIZE	((SINE_PULSE_DURATION + LINEAR_FRAGMENT_DURATION) / TIMER15_uS)

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)

#endif //  __GLOBAL_H
