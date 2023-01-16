#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "SysConfig.h"

// Definitions
// 
#define	SCCI_TIMEOUT_TICKS						1000	// Таймаут интерфейса SCCI (в мс)
#define EP_WRITE_COUNT							0		// Количество массивов для записи

#define EP_COUNT								6		// Количество массивов для чтения
#define ENABLE_LOCKING							FALSE	// Защита NV регистров паролем
#define CURRENT_PULSE_WIDTH						10000	// Длительность импульса тока (мкс)
#define CURRENT_PULSE_TAIL						5000	// Длительность хвоста импульса тока (мкс)
#define CURRENT_TAIL_START_CURR					30.0f	// Амплитуда тока для начала формирования хвоста (А)

#define PI										3.1416f	// Значение числа Пи
#define DAC_MAX_VAL								0x0FFF	// Максимальное значение ЦАП
#define	CURRENT_RANGE_QUANTITY					3		// Количество диапазонов измерения тока
#define CURRENT_RANGE_0							0		// Диапазон 0
#define CURRENT_RANGE_1							1		// Диапазон 1
#define CURRENT_RANGE_2							2		// Диапазон 2

// Размер массивов EP
#define VALUES_x_SIZE	((CURRENT_PULSE_WIDTH + CURRENT_PULSE_TAIL) / TIMER15_uS)

// Временные параметры
#define TIME_LED_BLINK							500		// Мигание светодиодом (в мс)

#endif //  __GLOBAL_H
