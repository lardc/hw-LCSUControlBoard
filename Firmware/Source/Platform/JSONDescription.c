#include "JSONDescription.h"
#include "FormatOutputJSON.h"

const char TemplateJSON[] = "[{\n"
"	'param' : 'Itm',\n"
"	'type' : 'set',\n"
"	'ranges': [\n"
"		{\n"
"			'rangeId' : '1',\n"
"			'unitsMultiply' : '1'\n"
"			'active' : '1',\n"
"			'min': '$',\n"
"			'max': '$'\n"
"		},\n"
"		{\n"
"			'rangeId' : '2',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '1',\n"
"			'min': '$',\n"
"			'max': '$'\n"
"		}]\n"
"}]";

float Itm1Min, Itm1Max, Itm2Min, Itm2Max;

void* JSONPointers[JSON_POINTERS_SIZE] = {0};

void JSON_AssignPointer(Int16U Index, pFloat32 Pointer)
{
	if (Index < JSON_POINTERS_SIZE)
		JSONPointers[Index] = Pointer;
}
