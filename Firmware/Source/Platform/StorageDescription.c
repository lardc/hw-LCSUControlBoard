// Header
#include "StorageDescription.h"
#include "Global.h"

// Variables
RecordDescription StorageDescription[] =
{
	{"REG_CURRENT_PULSE_VALUE", DT_Int16U,	1},

	{"REG_DEV_STATE",			DT_Int16U,	1},
	{"REG_FAULT_REASON",		DT_Int16U,	1},
	{"REG_DISABLE_REASON",		DT_Int16U,	1},
	{"REG_WARNING",				DT_Int16U,	1},
	{"REG_PROBLEM	",			DT_Int16U,	1},
	{"REG_OP_RESULT",			DT_Int16U,	1},
	{"REG_SUB_STATE",			DT_Int16U,	1},

	{"REG_RESULT_CURRENT",		DT_Int16U,	1},
	{"REG_BATTERY_VOLTAGE",		DT_Int16U,	1},

	{"Battery Voltage",			DT_Int16U,	VALUES_x_SIZE},
	{"Current",					DT_Int16U,	VALUES_x_SIZE},
	{"Regulator Output",		DT_Int16U,	VALUES_x_SIZE},
	{"Regulator Error",			DT_Int16U,	VALUES_x_SIZE},
	{"Current table",			DT_Int16U,	VALUES_x_SIZE},
	{"DAC Raw Data	",			DT_Int16U,	VALUES_x_SIZE},
	{"Values Counter",			DT_Int16U,	1}

};
Int32U TablePointers[sizeof(StorageDescription) / sizeof(StorageDescription[0])] = {0};
const Int16U StorageSize = sizeof(StorageDescription) / sizeof(StorageDescription[0]);
