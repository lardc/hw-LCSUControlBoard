// Include
#include "Interrupts.h"
//
#include "Controller.h"
#include "LowLevel.h"
#include "Board.h"
#include "SysConfig.h"
#include "Global.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"

// Variables
volatile bool INT_CurrentReady = false, INT_VBatReady = false;

// Forward functions
void INT_GeneralDMAHandler(DMA_TypeDef* DMAx, uint32_t Channel, volatile bool* Flag);

// Functions
//
void USART1_IRQHandler()
{
	if(ZwSCI_RecieveCheck(USART1))
	{
		ZwSCI_RegisterToFIFO(USART1);
		ZwSCI_RecieveFlagClear(USART1);
	}
}
//-----------------------------------------

void USB_LP_CAN_RX0_IRQHandler()
{
	if(NCAN_RecieveCheck())
	{
		NCAN_RecieveData();
		NCAN_RecieveFlagReset();
	}
}
//-----------------------------------------

void EXTI15_10_IRQHandler(void)
{
	CONTROL_ExternalInterruptProcess();

	EXTI_FlagReset(EXTI_10);
}
//-----------------------------------------------

void INT_GeneralDMAHandler(DMA_TypeDef* DMAx, uint32_t Channel, volatile bool* Flag)
{
	if(DMA_IsTransferComplete(DMAx, Channel))
	{
		DMA_TransferCompleteReset(DMAx, Channel);

		*Flag = true;

		if(INT_CurrentReady && INT_VBatReady)
		{
			CONTROL_HighPriorityProcess();
			INT_CurrentReady = INT_VBatReady = false;
		}
	}
}
//-----------------------------------------

void DMA1_Channel1_IRQHandler()
{
	INT_GeneralDMAHandler(DMA1, DMA_ISR_TCIF1, &INT_VBatReady);
	// Сброс флага OVR у ADC3 для непрерывной работы DMA2_Ch5
	if(ADC3->ISR & OVR)
		ADC3->ISR |= OVR;
}
//-----------------------------------------

void DMA2_Channel5_IRQHandler()
{
	INT_GeneralDMAHandler(DMA2, DMA_ISR_TCIF5, &INT_CurrentReady);
}
//-----------------------------------------

void TIM7_IRQHandler()
{
	static uint16_t LED_BlinkTimeCounter = 0;

	if(TIM_StatusCheck(TIM7))
	{
		CONTROL_HandleFanLogic(false);
		CONTROL_HandleExternalLamp(false);

		CONTROL_TimeCounter++;
		if(++LED_BlinkTimeCounter > TIME_LED_BLINK)
		{
			LL_ToggleBoardLED();
			LED_BlinkTimeCounter = 0;
		}

		TIM_StatusClear(TIM7);
	}
}
//-----------------------------------------
