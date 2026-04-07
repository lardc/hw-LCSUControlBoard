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
#include "ConvertUtils.h"
#include "SaveToFlash.h"
#include "Constraints.h"
#include "InitConfig.h"
#include "JSONDescription.h"

// Variables
//
volatile DeviceState CONTROL_State = DS_None;
volatile DeviceSubState CONTROL_SubState = SS_None;
static Boolean CycleActive = false;
static Boolean RequestSaveToFlash = FALSE;
//
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U	CONTROL_AfterPulsePause = 0;
volatile Int64U	CONTROL_BatteryChargeTimeCounter = 0;
volatile Int64U CONTROL_ConfigStateCounter = 0;
volatile Int16U CONTROL_Values_Counter = 0;
volatile Int16U CONTROL_ExtInfoCounter = 0;
volatile float 	CONTROL_ValuesCurrent[VALUES_x_SIZE];
volatile float  CONTROL_RegulatorErr[VALUES_x_SIZE];
volatile float  CONTROL_ValuesBatteryVoltage[VALUES_x_SIZE];
volatile float  CONTROL_RegulatorOutput[VALUES_x_SIZE];
volatile float  CONTROL_CurentTable[VALUES_x_SIZE];
volatile float  CONTROL_DACRawData[VALUES_x_SIZE];
volatile float  CONTROL_ExtInfoData[VALUES_EXT_INFO_SIZE];
//
float CONTROL_CurrentTarget;

// Forward functions
//
static Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U pUserError);
void CONTROL_SwitchToFault(Int16U Reason);
void CONTROL_UpdateWatchDog();
void CONTROL_ResetToDefaultState();
void CONTROL_LogicProcess();
void CONTROL_ResetOutputRegisters();
bool CONTROL_StartPrepareCached();
bool CONTROL_SwitchCurrentRangeRelayCached();
bool CONTROL_BatteryVoltageCheck();
void CONTROL_InitStoragePointers();
void CONTROL_SetProblem(Int16U Problem);
void CONTROL_ImpulseAmplitudeValues();

// Functions
//
void CONTROL_Init()
{
	// Переменные для конфигурации EndPoint
	Int16U FEPIndexes[FEP_COUNT] = {EP_CURRENT, EP_BATTERY_VOLTAGE, EP_REGULATOR_OUTPUT, EP_REGULATOR_ERR, EP_CUR_TABLE,
			EP_DAC_RAW_DATA, EP_ExtInfoData};

	Int16U FEPSized[FEP_COUNT] =
			{VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_EXT_INFO_SIZE};

	pInt16U FEPCounters[FEP_COUNT] = {(pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter,
			(pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_Values_Counter,
			(pInt16U)&CONTROL_Values_Counter, (pInt16U)&CONTROL_ExtInfoCounter};

	pFloat32 FEPDatas[FEP_COUNT] = {(pFloat32)CONTROL_ValuesCurrent, (pFloat32)CONTROL_ValuesBatteryVoltage,
			(pFloat32)CONTROL_RegulatorOutput, (pFloat32)CONTROL_RegulatorErr, (pFloat32)CONTROL_CurentTable,
			(pFloat32)CONTROL_DACRawData, (pFloat32)CONTROL_ExtInfoData};

	// Конфигурация сервиса работы DataTable и EPROM
	EPROMServiceConfig EPROMService = {(FUNC_EPROM_WriteValues)&NFLASH_WriteDT, (FUNC_EPROM_ReadValues)&NFLASH_ReadDT};
	// Инициализация DataTable
	DT_Init(EPROMService, false);

	// Инициализация функций связанных с CAN NodeID
	Int16U NodeID = DataTable[REG_CFG_NODE_ID] ? DataTable[REG_CFG_NODE_ID] : CAN_SLAVE_NID;
	DT_SaveFirmwareInfo(NodeID, 0);
	INITCFG_ConfigCANFilters(NodeID);

	// Инициализация device profile
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive, NodeID);
	DEVPROFILE_InitFEPService(FEPIndexes, FEPSized, FEPCounters, FEPDatas);
	// Сброс значений
	DEVPROFILE_ResetControlSection();
	CONTROL_ResetToDefaultState();

	CU_LoadConvertParams();

	// Инициализация указателей на сохраняемые данные
	CONTROL_InitStoragePointers();
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
	DataTable[REG_RESULT_MAX_DAC] = 0;

	DEVPROFILE_ResetScopes(0);
	DEVPROFILE_ResetEPReadState();
}
//------------------------------------------

void CONTROL_ResetToDefaultState()
{
	CONTROL_ResetOutputRegisters();
	
	LL_CurrentBoardLock(true);
	LL_PowerSupplyEnable(false);

	CONTROL_SetDeviceState(DS_None, SS_None);
}
//------------------------------------------

void CONTROL_Idle()
{
	CONTROL_LogicProcess();

	if (RequestSaveToFlash)
	{
		RequestSaveToFlash = FALSE;
		STF_SaveDiagData();
	}

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
				CONTROL_SetProblem(PROBLEM_MANUAL_STOP);
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
	static Int64U PulsePrepareDelayTimer = 0;

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
					CONTROL_SwitchToFault(DF_PROBLEM_BATTERY);
				}
			}
			break;

		case SS_PulsePrepare:
			if(CONTROL_StartPrepareCached())
				CONTROL_SetDeviceState(DS_ConfigReady, SS_None);
			else
			{
				PulsePrepareDelayTimer = CONTROL_TimeCounter + CURRENT_RANGE_CHANGE_DELAY;
				CONTROL_SetDeviceState(DS_InProcess, SS_PulsePrepareDelay);
			}
			break;

		case SS_PulsePrepareDelay:
			if(CONTROL_TimeCounter > PulsePrepareDelayTimer)
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
						CONTROL_SwitchToFault(DF_PROBLEM_BATTERY);
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
	if(CONTROL_State == DS_ConfigReady || CONTROL_SubState == SS_Pulse)
		return false;

	DataTable[REG_BATTERY_VOLTAGE] = MEASURE_SingleSampleBatteryVoltage();
	return (DataTable[REG_BATTERY_VOLTAGE] >= DataTable[REG_BATTERY_VOLTAGE_THRESHOLD]);
}
//-----------------------------------------------

void CONTROL_HighPriorityProcess()
{
	if(CONTROL_SubState == SS_Pulse)
	{
		Int16U Problem = PROBLEM_NONE;
		if(REGULATOR_Process(&Problem))
		{
			CONTROL_StopProcess();
			CONTROL_SetDeviceState(DS_InProcess, SS_WaitAfterPulse);

			// Завершения регулятора с проблемой
			if(Problem != PROBLEM_NONE)
			{
				CONTROL_SetProblem(Problem);
				return;
			}

			CONTROL_ImpulseAmplitudeValues();
		}
	}
}
//-----------------------------------------------

void CONTROL_SetProblem(Int16U Problem)
{
	DataTable[REG_PROBLEM] = Problem;
	DataTable[REG_OP_RESULT] = OPRESULT_FAIL;
}
//-----------------------------------------------

void CONTROL_ImpulseAmplitudeValues()
{
	if(DataTable[REG_PULSE_SHAPE] == PSH_Trapeze)
	{
		// Проверка наличия индекса завершения полки трапеции
		if(REGULATOR_FlattopLastIndex < (RESULT_TRAPEZE_POINTS - 1))
		{
			CONTROL_SetProblem(PROBLEM_TRAPEZE_INDEX);
			return;
		}

		float AvgDAC = 0, AvgCurrent = 0;
		for(Int16U i = 0; i < RESULT_TRAPEZE_POINTS; i++)
		{
			Int16U idx = REGULATOR_FlattopLastIndex - i;
			AvgDAC += CONTROL_DACRawData[idx];
			AvgCurrent += CONTROL_ValuesCurrent[idx];
		}

		DataTable[REG_RESULT_CURRENT] = AvgCurrent / RESULT_TRAPEZE_POINTS;
		DataTable[REG_RESULT_MAX_DAC] = AvgDAC / RESULT_TRAPEZE_POINTS;
		DataTable[REG_OP_RESULT] = OPRESULT_OK;
	}
	else
	{
		float AvgCurrent = 0, AvgDAC = 0;
		bool Result = MEASURE_FindMax((float *)CONTROL_ValuesCurrent, (float *)CONTROL_DACRawData,
				CONTROL_Values_Counter, &AvgDAC, &AvgCurrent);
		if(Result)
		{
			DataTable[REG_RESULT_CURRENT] = AvgCurrent;
			DataTable[REG_RESULT_MAX_DAC] = AvgDAC;
			DataTable[REG_OP_RESULT] = OPRESULT_OK;
		}
		else
			CONTROL_SetProblem(PROBLEM_SIN_CALC_FAIL);
	}
}
//-----------------------------------------------

bool CONTROL_StartPrepareCached()
{
	CONTROL_CurrentTarget = DataTable[REG_CURRENT_PULSE_VALUE];

	CU_LoadConvertParams();
	REGULATOR_CacheVariables();
	return CONTROL_SwitchCurrentRangeRelayCached();
}
//-----------------------------------------------

Int16U CONTROL_GetCurrentRange()
{
	if(CONTROL_CurrentTarget <= DataTable[REG_CURRENT_THRESHOLD_LOW])
	{
		return CurrentRange0;
	}
	else if(CONTROL_CurrentTarget <= DataTable[REG_CURRENT_THRESHOLD_HIGH])
	{
		return CurrentRange1;
	}
	else
	{
		return CurrentRange2;
	}
}
//-----------------------------------------------

bool CONTROL_SwitchCurrentRangeRelayCached()
{
	static CurrentRanges PrevRange = CurrentRangeUndef;
	CurrentRanges NewRange = CONTROL_GetCurrentRange();

	if(PrevRange == NewRange)
		return true;
	else
	{
		PrevRange = NewRange;
		if(CurrentRange0 == NewRange)
			LL_SetCurrentRange0();
		else
			LL_SetCurrentRange1();
		return false;
	}
}
//-----------------------------------------------

void CONTROL_StopProcess()
{
	TIM_Stop(TIM15);
	LL_WriteDAC(0);
	LL_CurrentBoardLock(true);
	LL_OutputAmplifierOffset(true);
	INITCFG_ADC1SoftTrig(true);

	float AfterPulseCoefficient = CONTROL_CurrentTarget
			/ (DataTable[REG_CURRENT_PER_CURBOARD] * DataTable[REG_CURBOARDS]);
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
		CONTROL_SetProblem(PROBLEM_SYNC_STOP);
	}
}
//------------------------------------------

void CONTROL_StartProcess()
{
	CONTROL_HandleFanLogic(true);
	CONTROL_HandleExternalLamp(true);

	LL_OutputAmplifierOffset(false);
	LL_CurrentBoardLock(false);
	DELAY_US(DAC_UNLOCK_STAB_TIME);

	INITCFG_ADC1SoftTrig(false);
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

void CONTROL_InitStoragePointers()
{
	STF_AssignPointer(0, (Int32U)&DataTable[REG_CURRENT_PULSE_VALUE]);

	STF_AssignPointer(1, (Int32U)&DataTable[REG_DEV_STATE]);
	STF_AssignPointer(2, (Int32U)&DataTable[REG_FAULT_REASON]);
	STF_AssignPointer(3, (Int32U)&DataTable[REG_DISABLE_REASON]);
	STF_AssignPointer(4, (Int32U)&DataTable[REG_WARNING]);
	STF_AssignPointer(5, (Int32U)&DataTable[REG_PROBLEM]);
	STF_AssignPointer(6, (Int32U)&DataTable[REG_OP_RESULT]);
	STF_AssignPointer(7, (Int32U)&DataTable[REG_SUB_STATE]);

	STF_AssignPointer(8, (Int32U)&DataTable[REG_RESULT_CURRENT]);
	STF_AssignPointer(9, (Int32U)&DataTable[REG_BATTERY_VOLTAGE]);

	STF_AssignPointer(10, (Int32U)CONTROL_ValuesBatteryVoltage);
	STF_AssignPointer(11, (Int32U)CONTROL_ValuesCurrent);
	STF_AssignPointer(12, (Int32U)CONTROL_RegulatorOutput);
	STF_AssignPointer(13, (Int32U)CONTROL_RegulatorErr);
	STF_AssignPointer(14, (Int32U)CONTROL_CurentTable);
	STF_AssignPointer(15, (Int32U)CONTROL_DACRawData);
	STF_AssignPointer(16, (Int32U)&CONTROL_Values_Counter);
}
//------------------------------------------

void CONTROL_InitJSONPointers()
{
	Itm1Min = DataTable[REG_LOW_ITM_LIMIT] ? DataTable[REG_LOW_ITM_LIMIT] : CURRENT_SETPOINT_MIN;
	Itm1Max = DataTable[REG_CURRENT_THRESHOLD_HIGH];

	Itm2Min = DataTable[REG_CURRENT_THRESHOLD_HIGH];
	Itm2Max = DataTable[REG_HIGH_ITM_LIMIT] ? DataTable[REG_HIGH_ITM_LIMIT] : CURRENT_SETPOINT_MAX;

	JSON_AssignPointer(0, &Itm1Min);
	JSON_AssignPointer(1, &Itm1Max);

	JSON_AssignPointer(2, &Itm2Min);
	JSON_AssignPointer(3, &Itm2Max);
}
//------------------------------------------
