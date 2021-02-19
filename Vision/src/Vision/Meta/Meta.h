#pragma once

#include "Vision/Core/Common.h"

enum MetaType
{
	MetaType_bool,
	MetaType_char,
	MetaType_int8,
	MetaType_uint8,
	MetaType_int16,
	MetaType_uint16,
	MetaType_int32,
	MetaType_uint32,
	MetaType_int64,
	MetaType_uint64,
	MetaType_float32,
	MetaType_float64,
	MetaType_Vector2,
	MetaType_Vector3,
	MetaType_DummyComponent,
};

struct StructMember
{
	const char* Name;
	MetaType    Type;
	uint32      Offset;
	uint32      ArrayCount;
	bool        IsPointer;
	bool        IsArray;
};

