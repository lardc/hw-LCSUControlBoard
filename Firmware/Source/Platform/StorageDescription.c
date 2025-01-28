// Header
#include "StorageDescription.h"
#include "Global.h"

// Variables
RecordDescription StorageDescription[] =
{
	{"REG_CURRENT_PULSE_VALUE", DT_Float,	1},

	{"REG_DEV_STATE",			DT_Float,	1},
	{"REG_FAULT_REASON",		DT_Float,	1},
	{"REG_DISABLE_REASON",		DT_Float,	1},
	{"REG_WARNING",				DT_Float,	1},
	{"REG_PROBLEM	",			DT_Float,	1},
	{"REG_OP_RESULT",			DT_Float,	1},
	{"REG_SUB_STATE",			DT_Float,	1},

	{"REG_RESULT_CURRENT",		DT_Float,	1},
	{"REG_BATTERY_VOLTAGE",		DT_Float,	1},

	{"Battery Voltage",			DT_Float,	VALUES_x_SIZE},
	{"Current",					DT_Float,	VALUES_x_SIZE},
	{"Regulator Output",		DT_Float,	VALUES_x_SIZE},
	{"Regulator Error",			DT_Float,	VALUES_x_SIZE},
	{"Current table",			DT_Float,	VALUES_x_SIZE},
	{"DAC Raw Data	",			DT_Float,	VALUES_x_SIZE},
	{"Values Counter",			DT_Int16U,	1}

};
Int32U TablePointers[sizeof(StorageDescription) / sizeof(StorageDescription[0])] = {0};
const Int16U StorageSize = sizeof(StorageDescription) / sizeof(StorageDescription[0]);
