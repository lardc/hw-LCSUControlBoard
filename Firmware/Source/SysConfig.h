#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H


// Flash loader options
#define BOOT_LOADER_VARIABLE			(*((volatile uint32_t *)0x20000000))
#define BOOT_LOADER_REQUEST				0x12345678
#define BOOT_LOADER_MAIN_PR_ADDR		0x08008000
//-----------------------------------------------

// System clock
#define SYSCLK							70000000	// Тактовая частота системной шины процессора
#define QUARTZ_FREQUENCY				20000000	// Частота кварца
// ----------------------------------------------

// USART
#define USART_BAUDRATE					115200		// Скорость USART
#define USART_FIFOlen					32			// Длина FIFO USART
// ----------------------------------------------

// Timers
#define TIMER15_uS						50			// в мкс
#define TIMER7_uS						1000		// в мкс
#define TIMER6_uS						10			// в мкс
// ----------------------------------------------

// CAN
#define CAN_BAUDRATE					1000000		// Битрейт CAN
// ----------------------------------------------

// ADC
#define DMA_ADC_CURRENT_CHANNEL			DMA2_Channel5	// Канал DMA для АЦП тока
#define DMA_ADC_V_BAT_CHANNEL			DMA1_Channel1	// Канал DMA для АЦП напряжения батареи
#define ADC1_V_BAT_CHANNEL				4				// АЦП1 номер канала напряжения
#define ADC3_CURRENT_CHANNEL			5				// АЦП1 номер канала тока
#define ADC_DMA_BUFF_SIZE				5				// Количество данных для DMA
// ----------------------------------------------

#endif // __SYSCONFIG_H
