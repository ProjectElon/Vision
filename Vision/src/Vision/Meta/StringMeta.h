#pragma once

#include "Vision/Meta/Meta.h"

#include "Vision/Core/String.h"

static StructMember StringMembers[] = 
{
	{ "Text", MetaType_char, offsetof(Vision::String, Text), 1, 1, 0 },
	{ "Count", MetaType_uint32, offsetof(Vision::String, Count), 1, 0, 0 },
};

