// Header
#include "Diagnostic.h"

// Includes
#include "DataTable.h"
#include "LowLevel.h"
#include "Controller.h"
#include "DebugActions.h"

// Functions
bool DIAG_HandleDiagnosticAction(uint16_t ActionID, uint16_t *pUserError)
{
	switch (ActionID)
	{
		case ACT_DBG_POWER_SUPPLY_CTRL:
			{
				if(CONTROL_State == DS_None)
					DBGACT_PowerSupplyEnable(DataTable[REG_DBG]);
				else
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;

		case ACT_DBG_PULSE:
			{
				if(CONTROL_State == DS_None)
					DBGACT_PulseProcess(DataTable[REG_DBG]);
				else
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;

		case ACT_DBG_SET_CURRENT_RANGE:
			{
				if(CONTROL_State == DS_None)
					DBGACT_SetCurrentRange(DataTable[REG_DBG]);
				else
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;

		case ACT_DBG_FAN_CTRL:
			{
				if(CONTROL_State == DS_None)
					DBGACT_FanControl(DataTable[REG_DBG]);
				else
					*pUserError = ERR_OPERATION_BLOCKED;
			}
			break;

		default:
			return false;
	}

	return true;
}
