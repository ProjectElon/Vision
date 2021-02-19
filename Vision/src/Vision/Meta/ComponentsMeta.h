#pragma once

#include "Vision/Meta/Meta.h"

StructMember Vector2Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector2, x), 1, 0, 0 },
	{ "y", MetaType_float32, offsetof(Vision::Vector2, y), 1, 0, 0 },
};

StructMember Vector3Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector3, x), 1, 0, 0 },
	{ "y", MetaType_float32, offsetof(Vision::Vector3, y), 1, 0, 0 },
	{ "z", MetaType_float32, offsetof(Vision::Vector3, z), 1, 0, 0 },
};

StructMember DummyComponentMembers[] = 
{
	{ "Name", MetaType_char, offsetof(Vision::DummyComponent, Name), 1, 1, 0 },
	{ "EntityID", MetaType_uint32, offsetof(Vision::DummyComponent, EntityID), 1, 0, 0 },
	{ "Position", MetaType_Vector2, offsetof(Vision::DummyComponent, Position), 1, 0, 0 },
	{ "Direction", MetaType_Vector3, offsetof(Vision::DummyComponent, Direction), 1, 0, 0 },
	{ "IsFacingRight", MetaType_bool, offsetof(Vision::DummyComponent, IsFacingRight), 1, 0, 0 },
	{ "RisingFactor", MetaType_float32, offsetof(Vision::DummyComponent, RisingFactor), 1, 0, 0 },
};

