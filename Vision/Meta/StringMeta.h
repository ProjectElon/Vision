#pragma once

#include "Meta.h"

#include "../src/Vision/Core/String.h"

static StructMember StringMembers[] = 
{
	{ "Data", MetaType_char, offsetof(Vision::String, Data), 0, sizeof(char), 24 },
	{ "Count", MetaType_size_t, offsetof(Vision::String, Count), 0, sizeof(size_t), 8 },
};

