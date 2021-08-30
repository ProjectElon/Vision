#pragma once

#include "Meta.h"

#include "../src/Vision/Core/Vars.h"

static StructMember SettingsMembers[] = 
{
	{ "WindowX", MetaType_int32, offsetof(Vision::Settings, WindowX), 0, sizeof(int32), 8 },
	{ "WindowY", MetaType_int32, offsetof(Vision::Settings, WindowY), 0, sizeof(int32), 8 },
	{ "WindowWidth", MetaType_uint32, offsetof(Vision::Settings, WindowWidth), 0, sizeof(uint32), 8 },
	{ "WindowHeight", MetaType_uint32, offsetof(Vision::Settings, WindowHeight), 0, sizeof(uint32), 8 },
	{ "ViewportWidth", MetaType_uint32, offsetof(Vision::Settings, ViewportWidth), 0, sizeof(uint32), 8 },
	{ "ViewportHeight", MetaType_uint32, offsetof(Vision::Settings, ViewportHeight), 0, sizeof(uint32), 8 },
	{ "RendererAPI", MetaType_String, offsetof(Vision::Settings, RendererAPI), 0, sizeof(Vision::String), 0 },
	{ "WindowTitle", MetaType_String, offsetof(Vision::Settings, WindowTitle), 0, sizeof(Vision::String), 0 },
	{ "ScenePath", MetaType_String, offsetof(Vision::Settings, ScenePath), 0, sizeof(Vision::String), 0 },
	{ "FontSize", MetaType_uint32, offsetof(Vision::Settings, FontSize), 0, sizeof(uint32), 8 },
	{ "DebugFontSize", MetaType_uint32, offsetof(Vision::Settings, DebugFontSize), 0, sizeof(uint32), 8 },
	{ "MainFontPath", MetaType_String, offsetof(Vision::Settings, MainFontPath), 0, sizeof(Vision::String), 0 },
	{ "DebugFontPath", MetaType_String, offsetof(Vision::Settings, DebugFontPath), 0, sizeof(Vision::String), 0 },
};

static StructMember VarsMembers[] = 
{
	{ "Settings", MetaType_Settings, offsetof(Vision::Vars, Settings), 0, sizeof(Vision::Settings), 0 },
};

