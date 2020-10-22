#pragma once

#include "pch.h"
#include "Vision/Core/Base.h"
#include "Vision/Platform/KeyCodes.h"
#include "Vision/Platform/MouseCodes.h"

#include <glm/glm.hpp>

namespace Vision
{
	class Input
	{
	public:
		static void Init();
		
		static bool IsKeyDown(uint32 keyCode);
		static bool IsKeyUp(uint32 keyCode);
		
		static bool IsMouseButtonDown(uint32 button);
		static bool IsMouseButtonUp(uint32 button);
		static glm::vec2 GetMousePosition();
		
		static bool IsCursorHovering();
		static void SetCursorVisibility(bool visible);
	};
}