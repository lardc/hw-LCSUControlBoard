// Header
#include "Controller.h"
//
// Includes
#include "Board.h"
#include "Delay.h"
#include "DeviceProfile.h"
#include "Interrupts.h"
#include "LowLevel.h"
#include "SysConfig.h"
#include "DebugActions.h"
#include "Diagnostic.h"
#include "BCCIxParams.h"
#include "Measurement.h"
#include "math.h"


// Variables
//
volatile DeviceState CONTROL_State = DS_None;
volatile DeviceSubState CONTROL_SubState = SS_None;
static Boolean CycleActive = false;
//
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U	CONTROL_AfterPulsePause = 0;
volatile Int64U	CONTROL_BatteryChargeTimeCounter = 0;
volatile Int64U CONTROL_ConfigStateCounter = 0;
volatile Int16U CONTROL_Values_Counter = 0;
volatile float 	CONTROL_ValuesCurrent[VALUES_x_SIZE];
volatile float  CONTROL_RegulatorErr[VALUES_x_SIZE];
volatile float  CONTROL_ValuesBatteryVoltage[VALUES_x_SIZE];
volatile float  CONTROL_RegulatorOutput[VALUES_x_SIZE];
volatile float  CONTROL_CurentTable[VALUES_x_SIZE];
volatile float  CONTROL_DACRawData[VALUES_x_SIZE];
//
float CONTROL_CurrentMaxValue = 0;
//
volatile RegulatorParamsStruct RegulatorParams;

/// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_UpdateWatchDog();
void CONTROL_ResetToDefaultState();
void CONTROL_LogicProcess();
void CONTROL_StopProcess();
void CONTROL_ResetOutputRegisters();
bool CONTROL_RegulatorCycle(volatile RegulatorParamsStruct* Regulator);
void CONTROL_StartPrepare();
void CONTROL_CashVariables();
bool CONTROL_BatteryVoltageCheck();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U FEPIndexes[FEP_COUNT] = {EP_CURRENT, EP_BATTERY_VOLTAGE, EP_REGULATOR_OUTPUT, EP_REGULATOR_ERR, EP_CUR_TABLE,
			EP_DAC_RAW_DATA};

	Int16U FEPSized[FEP_COUNT] =
			{VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE};

	pInt16U FEPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter,
			(pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter,
			(pInt16U)&CONTROL_Values_Counter};

	pFloat32 FEPDatas[FEP_COUNT] = {(pFloat32)&CONTROL_ValuesCurrent, (pFloat32)&CONTROL_ValuesBatteryVoltage,
			(pFloat32)&CONTROL_RegulatorOutput, (pFloat32)&CONTROL_RegulatorErr, (pFloat32)&CONTROL_CurentTable,
			(pFloat32)&CONTROL_DACRawData};

	// Конфигурация сервиса работы Data-table и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация data table
	DT_Init(EPROMService, false);
	DT_SaveFirmwareInfo(CAN_SLAVE_NID, 0);

	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitFEPService(FEPIndexes, FEPSized, FEPCounters, FEPDatas);
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaultState();

	CU_LoadConvertParams();
}
//------------------------------------------

void CONTROL_ResetOutputRegisters()
{
	DataTable[REG_FAULT_REASON] = DF_NONE;
	DataTable[REG_DISABLE_REASON] = DF_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;
	DataTable[REG_PROBLEM] = PROBLEM_NONE;
	DataTable[REG_OP_RESULT] = OPRESULT_NONE;
	
	DataTable[REG_RESULT_CURRENT] = 0;

	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//------------------------------------------

void CONTROL_ResetToDefaultState()
{
	CONTROL_ResetOutputRegisters();
	
	LL_LSLCurrentBoardLock(true);
	LL_PowerSupplyEnable(false);

	CONTROL_SetDeviceState(DS_None, SS_None);
}
//------------------------------------------

void CONTROL_Idle()
{
	CONTROL_LogicProcess();

	DEVPROFILE_ProcessRequests();
	CONTROL_UpdateWatchDog();
}
//------------------------------------------

static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError)
{
	*pUserError = ERR_NONE;
	
	switch (ActionID)
	{
		case ACT_ENABLE_POWER:
			if(CONTROL_State == DS_None)
			{
				CONTROL_BatteryChargeTimeCounter = CONTROL_TimeCounter + DataTable[REG_BATTERY_FULL_CHRAGE_TIMEOUT];
				CONTROL_SetDeviceState(DS_InProcess, SS_PowerPrepare);
				LL_PowerSupplyEnable(true);
			}
			else if(CONTROL_State != DS_Ready)
				*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_DISABLE_POWER:
			if((CONTROL_State == DS_Ready) || ((CONTROL_State == DS_InProcess) && (CONTROL_SubState == SS_PowerPrepare)))
			{
				CONTROL_ResetToDefaultState();
			}
			else if(CONTROL_State != DS_None)
					*pUserError = ERR_OPERATION_BLOCKED;
			break;

		case ACT_CONFIG_UNIT:
			if (CONTROL_State == DS_Ready)
			{
				CONTROL_ResetOutputRegisters();
				CONTROL_ConfigStateCounter = CONTROL_TimeCounter + DataTable[REG_CONFIG_READY_STATE_TIMEOUT];
				CONTROL_SetDeviceState(DS_InProcess, SS_PulsePrepare);
			}
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		case ACT_SOFTWARE_START:
			if (CONTROL_State == DS_ConfigReady)
			{
				CONTROL_SetDeviceState(DS_InProcess, SS_Pulse);
				CONTROL_StartProcess();
			}
			else
				if (CONTROL_State == DS_InProcess)
					*pUserError = ERR_OPERATION_BLOCKED;
				else
					*pUserError = ERR_DEVICE_NOT_READY;
			break;

		case ACT_STOP_PROCESS:
			if (CONTROL_State == DS_InProcess)
			{
				CONTROL_StopProcess();
				CONTROL_SetDeviceState(DS_Ready, SS_None);
			}
			break;

		case ACT_CLR_FAULT:
			if (CONTROL_State == DS_Fault)
			{
				CONTROL_SetDeviceState(DS_None, SS_None);
				DataTable[REG_FAULT_REASON] = DF_NONE;
			}
			break;

		case ACT_CLR_WARNING:
			DataTable[REG_WARNING] = WARNING_NONE;
			break;

		default:
			return DIAG_HandleDiagnosticAction(ActionID, pUserError);
			
	}
	return true;
}
//-----------------------------------------------

void CONTROL_LogicProcess()
{
	switch(CONTROL_SubState)
	{
		case SS_PowerPrepare:
			if(CONTROL_BatteryVoltageCheck())
				CONTROL_SetDeviceState(DS_Ready, SS_None);
			else
			{
				if(CONTROL_TimeCounter >= CONTROL_BatteryChargeTimeCounter)
				{
					CONTROL_ResetToDefaultState();
					CONTROL_SwitchToFault(PROBLEM_BATTERY);
				}
			}
			break;

		case SS_PulsePrepare:
			CONTROL_StartPrepare();
			CONTROL_SetDeviceState(DS_ConfigReady, SS_None);
			break;

		case SS_WaitAfterPulse:
			if(CONTROL_TimeCounter > CONTROL_AfterPulsePause)
			{
				if(CONTROL_BatteryVoltageCheck())
					CONTROL_SetDeviceState(DS_Ready, SS_None);
				else
				{
					if(CONTROL_TimeCounter >= CONTROL_BatteryChargeTimeCounter)
					{
						CONTROL_ResetToDefaultState();
						CONTROL_SwitchToFault(PROBLEM_BATTERY);
					}
				}
			}
			break;

		default:
			CONTROL_BatteryVoltageCheck();

			if((CONTROL_State == DS_ConfigReady) && (CONTROL_TimeCounter >= CONTROL_ConfigStateCounter))
				CONTROL_SetDeviceState(DS_Ready, SS_None);
			break;
	}
}
//-----------------------------------------------

bool CONTROL_BatteryVoltageCheck()
{
	DataTable[REG_BATTERY_VOLTAGE] = MEASURE_SingleSampleBatteryVoltage();

	if(DataTable[REG_BATTERY_VOLTAGE] < DataTable[REG_BATTERY_VOLTAGE_THRESHOLD])
		return false;
	else
		return true;
}
//-----------------------------------------------

void CONTROL_HighPriorityProcess()
{
	if(CONTROL_SubState == SS_Pulse)
	{
		MEASURE_SampleParams(&RegulatorParams);

		if(CONTROL_RegulatorCycle(&RegulatorParams))
		{
			CONTROL_StopProcess();
			CONTROL_SetDeviceState(DS_InProcess, SS_WaitAfterPulse);
			DataTable[REG_OP_RESULT] = OPRESULT_OK;
		}
	}
}
//-----------------------------------------------

bool CONTROL_RegulatorCycle(volatile RegulatorParamsStruct* Regulator)
{
	return REGULATOR_Process(Regulator);
}
//-----------------------------------------------

void CONTROL_StartPrepare()
{
	MEASURE_DMABufferClear();
	CU_LoadConvertParams();
	REGULATOR_CashVariables(&RegulatorParams);
	CONTROL_CashVariables();
	MEASURE_SetCurrentRange(&RegulatorParams);
	CONTROL_PulseShapeConfig(&RegulatorParams);
	CONTROL_CopyCurrentToEP(&RegulatorParams);
}
//-----------------------------------------------

void CONTROL_CashVariables()
{
	RegulatorParams.CurrentTarget = DataTable[REG_CURRENT_PULSE_VALUE];

	CONTROL_CurrentMaxValue = DataTable[REG_CURRENT_PER_CURBOARD] * DataTable[REG_CURBOARDS];
	if(RegulatorParams.CurrentTarget > CONTROL_CurrentMaxValue)
		RegulatorParams.CurrentTarget = CONTROL_CurrentMaxValue;
}
//-----------------------------------------------

void CONTROL_PulseShapeConfig(volatile RegulatorParamsStruct* Regulator)
{
	switch((Int16U)(DataTable[REG_PULSE_SHAPE]))
	{
	case SINE_SHAPE:
		CONTROL_SineShapeConfig(Regulator);
		break;

	case MOD_SINE_SHAPE:
		CONTROL_ModSineShapeConfig(Regulator);
		break;

	case TRAPEZE_SHAPE:
		CONTROL_TrapezeShapeConfig(Regulator);
		break;
	}
}
//-----------------------------------------------

void CONTROL_SineShapeConfig(volatile RegulatorParamsStruct* Regulator)
{
	Regulator->PulseCounterMax = SINE_PULSE_DURATION / TIMER15_uS;

	for(int i = 0; i < Regulator->PulseCounterMax; ++i)
	{
		float Setpoint = Regulator->CurrentTarget * sin(PI * i / (Regulator->PulseCounterMax - 1));
		Regulator->CurrentTable[i] = (Setpoint > 0) ? Setpoint : 0;
	}
}
//-----------------------------------------------

void CONTROL_ModSineShapeConfig(volatile RegulatorParamsStruct* Regulator)
{
	float LinearCurrent = LINEAR_FRAGMENT_AMPLITUDE;
	Int16U LinearStartIndex = 0;

	Regulator->PulseCounterMax = SINE_PULSE_DURATION / TIMER15_uS;

	for(int i = 0; i < Regulator->PulseCounterMax; ++i)
	{
		float Setpoint = Regulator->CurrentTarget * sin(PI * i / (Regulator->PulseCounterMax - 1));
		Regulator->CurrentTable[i] = (Setpoint > 0) ? Setpoint : 0;

		if((i > Regulator->PulseCounterMax / 2) && (Regulator->CurrentTable[i] <= LinearCurrent))
		{
			LinearStartIndex = i;
			break;
		}
	}

	// Дописываем плавно спадающий хвост
	float dI = LinearCurrent / (SINE_PULSE_DURATION + LINEAR_FRAGMENT_DURATION - LinearStartIndex);
	for (int i = LinearStartIndex; i < PULSE_BUFFER_SIZE; ++i)
	{
		LinearCurrent -= dI;
		Regulator->CurrentTable[i] = LinearCurrent;
	}
}
//-----------------------------------------------

void CONTROL_TrapezeShapeConfig(volatile RegulatorParamsStruct* Regulator)
{
	float dI = 0, Setpoint = 0;
	Int16U EdgeIndex = 0;

	Regulator->PulseCounterMax = DataTable[REG_TRAPEZE_DURATION] / TIMER15_uS * 1000;
	dI = Regulator->CurrentTarget / DataTable[REG_TRAPEZE_CURRENT_RATE] / TIMER15_uS * 1000;

	for(int i = 0; i < Regulator->PulseCounterMax; ++i)
	{
		if(Setpoint < Regulator->CurrentTarget)
		{
			Regulator->CurrentTable[i] = Setpoint;
			Setpoint += dI;
		}
		else
		{
			if(!EdgeIndex)
				EdgeIndex = i;

			if(i < (Regulator->PulseCounterMax - EdgeIndex))
				Regulator->CurrentTable[i] = Regulator->CurrentTarget;
			else
			{
				Setpoint -= dI;
				Regulator->CurrentTable[i] = Setpoint;
			}
		}
	}
}
//-----------------------------------------------

void CONTROL_CopyCurrentToEP(volatile RegulatorParamsStruct* Regulator)
{
	for(int i = 0; i < PULSE_BUFFER_SIZE; ++i)
		CONTROL_CurentTable[i] = (Int16S)Regulator->CurrentTable[i];
}
//-----------------------------------------------

void CONTROL_StopProcess()
{
	TIM_Stop(TIM15);
	LL_WriteDAC(0);
	LL_LSLCurrentBoardLock(true);
	LL_OutputAmplifierOffset(true);

	float AfterPulseCoefficient = RegulatorParams.CurrentTarget / CONTROL_CurrentMaxValue;
	CONTROL_AfterPulsePause = CONTROL_TimeCounter + DataTable[REG_AFTER_PULSE_PAUSE] * AfterPulseCoefficient;
	CONTROL_BatteryChargeTimeCounter = CONTROL_TimeCounter + DataTable[REG_BATTERY_RECHARGE_TIMEOUT];
}
//------------------------------------------

void CONTROL_ExternalInterruptProcess()
{
	if (CONTROL_State == DS_ConfigReady && !LL_SyncLineGetState())
	{
		CONTROL_SetDeviceState(DS_InProcess, SS_Pulse);
		CONTROL_StartProcess();
	}

	if (CONTROL_SubState == SS_Pulse && LL_SyncLineGetState())
	{
		CONTROL_StopProcess();
		CONTROL_SetDeviceState(DS_InProcess, SS_WaitAfterPulse);
		DataTable[REG_OP_RESULT] = OPRESULT_OK;
	}
}
//------------------------------------------

void CONTROL_StartProcess()
{
	CONTROL_HandleFanLogic(true);
	CONTROL_HandleExternalLamp(true);

	LL_OutputAmplifierOffset(false);
	LL_LSLCurrentBoardLock(false);
	TIM_Reset(TIM15);
	TIM_Start(TIM15);
}
//-----------------------------------------------

void CONTROL_HandleFanLogic(bool IsImpulse)
{
	static uint32_t IncrementCounter = 0;
	static uint64_t FanOnTimeout = 0;

	if(DataTable[REG_FAN_CTRL])
	{
		// Увеличение счётчика в простое
		if (!IsImpulse)
			IncrementCounter++;

		// Включение вентилятора
		if ((IncrementCounter > ((uint32_t)DataTable[REG_FAN_OPERATE_PERIOD] * 1000)) || IsImpulse)
		{
			IncrementCounter = 0;
			FanOnTimeout = CONTROL_TimeCounter + ((uint32_t)DataTable[REG_FAN_OPERATE_TIME] * 1000);
			LL_Fan(true);
		}

		// Отключение вентилятора
		if (FanOnTimeout && (CONTROL_TimeCounter > FanOnTimeout))
		{
			FanOnTimeout = 0;
			LL_Fan(false);
		}
	}
	else
		LL_Fan(false);
}
//-----------------------------------------------

void CONTROL_HandleExternalLamp(bool IsImpulse)
{
	static Int64U ExternalLampCounter = 0;

	if(DataTable[REG_EXT_IND_CTRL])
	{
		if(CONTROL_State == DS_Fault)
		{
			if(++ExternalLampCounter > TIME_FAULT_EXT_LED_BLINK)
			{
				LL_ToggleExtIndication();
				ExternalLampCounter = 0;
			}
		}
		else
			{
				if(IsImpulse)
				{
					LL_ExtIndication(true);
					ExternalLampCounter = CONTROL_TimeCounter + EXT_LAMP_ON_STATE_TIME;
				}
				else
				{
					if(CONTROL_TimeCounter >= ExternalLampCounter)
						LL_ExtIndication(false);
				}
			}
	}
}
//-----------------------------------------------

void CONTROL_SwitchToFault(Int16U Reason)
{
	CONTROL_SetDeviceState(DS_Fault, SS_None);
	DataTable[REG_FAULT_REASON] = Reason;
}
//------------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState, DeviceSubState NewSubState)
{
	CONTROL_State = NewState;
	CONTROL_SubState = NewSubState;
	DataTable[REG_DEV_STATE] = NewState;
	DataTable[REG_SUB_STATE] = NewSubState;
}
//------------------------------------------

void CONTROL_UpdateWatchDog()
{
	if(BOOT_LOADER_VARIABLE != BOOT_LOADER_REQUEST)
		IWDG_Refresh();
}
//------------------------------------------

