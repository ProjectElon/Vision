#pragma once

#include "Vision/Meta/Meta.h"

#include "Vision/Core/Vars.h"

static StructMember SettingsMembers[] = 
{
	{ "WindowX", MetaType_int32, offsetof(Vision::Settings, WindowX), 1, 0, 0 },
	{ "WindowY", MetaType_int32, offsetof(Vision::Settings, WindowY), 1, 0, 0 },
	{ "WindowWidth", MetaType_uint32, offsetof(Vision::Settings, WindowWidth), 1, 0, 0 },
	{ "WindowHeight", MetaType_uint32, offsetof(Vision::Settings, WindowHeight), 1, 0, 0 },
	{ "ViewportWidth", MetaType_uint32, offsetof(Vision::Settings, ViewportWidth), 1, 0, 0 },
	{ "ViewportHeight", MetaType_uint32, offsetof(Vision::Settings, ViewportHeight), 1, 0, 0 },
	{ "FontSize", MetaType_uint32, offsetof(Vision::Settings, FontSize), 1, 0, 0 },
	{ "DebugFontSize", MetaType_uint32, offsetof(Vision::Settings, DebugFontSize), 1, 0, 0 },
	{ "IsWindowMaximized", MetaType_bool, offsetof(Vision::Settings, IsWindowMaximized), 1, 0, 0 },
	{ "FullScreen", MetaType_bool, offsetof(Vision::Settings, FullScreen), 1, 0, 0 },
	{ "WindowTitle", MetaType_String, offsetof(Vision::Settings, WindowTitle), 1, 0, 0 },
	{ "ScenePath", MetaType_String, offsetof(Vision::Settings, ScenePath), 1, 0, 0 },
};

static StructMember VarsMembers[] = 
{
	{ "Settings", MetaType_Settings, offsetof(Vision::Vars, Settings), 1, 0, 0 },
};

