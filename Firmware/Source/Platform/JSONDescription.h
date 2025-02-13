#ifndef __JSON_DESCRIPTION_H
#define __JSON_DESCRIPTION_H

#include "Global.h"

#define JSON_POINTERS_SIZE 32

extern float Itm1Min, Itm1Max, Itm2Min, Itm2Max;

extern pFloat32 JSONPointers[JSON_POINTERS_SIZE];

void JSON_AssignPointer(Int16U Index, pFloat32 Pointer);

#endif // __JSON_DESCRIPTION_H
