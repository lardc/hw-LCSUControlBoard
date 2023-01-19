#include "InitConfig.h"
#include "Board.h"
#include "SysConfig.h"
#include "BCCIxParams.h"
#include "Measurement.h"

// Functions
//
Boolean INITCFG_ConfigSystemClock()
{
	return RCC_PLL_HSE_Config(QUARTZ_FREQUENCY, PREDIV_4, PLL_14);
}
//------------------------------------------------

void INITCFG_ConfigDAC()
{
	DACx_Clk_Enable(DAC_1_ClkEN);
	DACx_Reset();
	DAC_TriggerConfigCh1(DAC1, TRIG1_TIMER6, TRIG1_ENABLE);
	DAC_BufferCh1(DAC1, false);
	DAC_EnableCh1(DAC1);
}
//------------------------------------------------

void INITCFG_ConfigIO()
{
	// Включение тактирования портов
	RCC_GPIO_Clk_EN(PORTA);
	RCC_GPIO_Clk_EN(PORTB);
	
	// Выходы
	GPIO_InitPushPullOutput(GPIO_FAN);
	GPIO_InitPushPullOutput(GPIO_PS_CTRL);
	GPIO_InitPushPullOutput(GPIO_LED);
	GPIO_InitPushPullOutput(GPIO_IND_CTRL);
	GPIO_InitPushPullOutput(GPIO_CURRENT_RANGE);

	// Выходы OpenDrain
	GPIO_InitOpenDrainOutput(GPIO_OUT_LOCK, NoPull);
	GPIO_InitOpenDrainOutput(GPIO_OFFSET_CTRL, NoPull);

	// Входы
	GPIO_InitInput(GPIO_SYNC, Pull_Up);

	// Начальная установка состояний выводов
	GPIO_SetState(GPIO_FAN, false);
	GPIO_SetState(GPIO_PS_CTRL, false);
	GPIO_SetState(GPIO_LED, false);
	GPIO_SetState(GPIO_IND_CTRL, false);
	GPIO_SetState(GPIO_CURRENT_RANGE, false);
	GPIO_SetState(GPIO_OUT_LOCK, true);
	GPIO_SetState(GPIO_OFFSET_CTRL, true);

	// Альтернативные функции
	GPIO_InitAltFunction(GPIO_ALT_CAN_RX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_CAN_TX, AltFn_9);
	GPIO_InitAltFunction(GPIO_ALT_UART1_RX, AltFn_7);
	GPIO_InitAltFunction(GPIO_ALT_UART1_TX, AltFn_7);
}

//------------------------------------------------
void INITCFG_ConfigCAN()
{
	RCC_CAN_Clk_EN(CAN_1_ClkEN);
	NCAN_Init(SYSCLK, CAN_BAUDRATE, FALSE);
	NCAN_FIFOInterrupt(TRUE);
	NCAN_FilterInit(0, CAN_SLAVE_FILTER_ID, CAN_SLAVE_FILTER_ID);
	NCAN_InterruptSetPriority(0);
}
//------------------------------------------------

void INITCFG_ConfigUART()
{
	USART_Init(USART1, SYSCLK, USART_BAUDRATE);
	USART_Recieve_Interupt(USART1, 0, true);
}
//------------------------------------------------

void INITCFG_ConfigTimer7()
{
	TIM_Clock_En(TIM_7);
	TIM_Config(TIM7, SYSCLK, TIMER7_uS);
	TIM_Interupt(TIM7, 2, true);
	TIM_Start(TIM7);
}
//------------------------------------------------

void INITCFG_ConfigTimer6()
{
	TIM_Clock_En(TIM_6);
	TIM_Config(TIM6, SYSCLK, TIMER6_uS);
	TIM_MasterMode(TIM6, MMS_UPDATE);
	TIM_Start(TIM6);
}
//------------------------------------------------

void INITCFG_ConfigTimer15()
{
	TIM_Clock_En(TIM_15);
	TIM_Config(TIM15, SYSCLK, TIMER15_uS);
	TIM_Interupt(TIM15, 1, true);
}
//------------------------------------------------

void INITCFG_ConfigWatchDog()
{
	IWDG_Config();
	IWDG_ConfigureSlowUpdate();
}
//------------------------------------------------

void INITCFG_ConfigADC()
{
	RCC_ADC_Clk_EN(ADC_12_ClkEN);
	RCC_ADC_Clk_EN(ADC_34_ClkEN);

	INITCFG_ADCConfigChannel(ADC1, ADC1_V_BAT_CHANNEL);
	INITCFG_ADCConfigChannel(ADC3, ADC3_CURRENT_CHANNEL_R0);
}
//------------------------------------------------

void INITCFG_ADCConfigChannel(ADC_TypeDef* ADCx, Int16U Channel)
{
	ADC_Calibration(ADCx);
	ADC_SoftTrigConfig(ADCx);
	ADC_ChannelSeqReset(ADCx);

	for (uint8_t i = 1; i <= ADC_DMA_BUFF_SIZE; ++i)
		ADC_ChannelSet_Sequence(ADCx, Channel, i);

	ADC_ChannelSeqLen(ADCx, ADC_DMA_BUFF_SIZE);
	ADC_DMAConfig(ADCx);
	ADC_Enable(ADCx);
}
//------------------------------------------------

void INITCFG_ConfigDMA()
{
	DMA_Clk_Enable(DMA1_ClkEN);
	DMA_Clk_Enable(DMA2_ClkEN);

	// DMA для АЦП напряжения батареи
	DMA_Reset(DMA_ADC_V_BAT_CHANNEL);
	DMAChannelX_Config(DMA_ADC_V_BAT_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
							DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_EN, DMA_READ_FROM_PERIPH);
	DMAChannelX_DataConfig(DMA_ADC_V_BAT_CHANNEL, (uint32_t)(&MEASURE_ADC_BatteryVoltageRaw[0]), (uint32_t)(&ADC1->DR), ADC_DMA_BUFF_SIZE);
	DMA_ChannelEnable(DMA_ADC_V_BAT_CHANNEL, true);

	// DMA для АЦП тока
	DMA_Reset(DMA_ADC_CURRENT_CHANNEL);
	DMAChannelX_Config(DMA_ADC_CURRENT_CHANNEL, DMA_MEM2MEM_DIS, DMA_LvlPriority_LOW, DMA_MSIZE_16BIT, DMA_PSIZE_16BIT,
							DMA_MINC_EN, DMA_PINC_DIS, DMA_CIRCMODE_EN, DMA_READ_FROM_PERIPH);
	DMAChannelX_DataConfig(DMA_ADC_CURRENT_CHANNEL, (uint32_t)(&MEASURE_ADC_CurrentRaw[0]), (uint32_t)(&ADC3->DR), ADC_DMA_BUFF_SIZE);
	DMA_ChannelEnable(DMA_ADC_CURRENT_CHANNEL, true);
}
//------------------------------------------------

void INITCFG_ConfigExtInterrupt()
{
	EXTI_Config(EXTI_PB, EXTI_10, BOTH_TRIG, 0);
	EXTI_EnableInterrupt(EXTI15_10_IRQn, 0, true);
}
//------------------------------------------------
