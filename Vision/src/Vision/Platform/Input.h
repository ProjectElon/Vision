#pragma once

#include "pch.h"
#include "Vision/Core/Base.h"
#include "Vision/Platform/KeyCodes.h"
#include "Vision/Platform/MouseCodes.h"

namespace Vision
{
	class Input
	{
	public:
		static void Init();
		static void ShutDown();

		inline static bool IsKeyDown(unsigned int keyCode) { return s_Instance->IsKeyDownImp(keyCode); }
		inline static bool IsKeyUp(unsigned int keyCode)   { return s_Instance->IsKeyUpImp(keyCode); }
		
		inline static bool IsMouseButtonDown(unsigned int button) { return s_Instance->IsMouseButtonDownImp(button); }
		inline static bool IsMouseButtonUp(unsigned int button)   { return s_Instance->IsMouseButtonUpImp(button); }
		inline static std::pair<float, float> GetMousePosition()  { return s_Instance->GetMousePositionImp(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImp(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImp(); }

		inline static bool IsCursorHovering()          { s_Instance->IsCursorHoveringImp(); }
		inline static void SetCursorVisibility(bool visible) { s_Instance->SetCursorVisibilityImp(visible); }

	protected:
		virtual bool IsKeyDownImp(unsigned int keyCode) const = 0;
		virtual bool IsKeyUpImp(unsigned int keyCode) const = 0;
		
		virtual bool IsMouseButtonDownImp(unsigned int button) const = 0;
		virtual bool IsMouseButtonUpImp(unsigned int button) const = 0;
		virtual std::pair<float, float> GetMousePositionImp() const = 0;
		virtual float GetMouseXImp() const = 0;
		virtual float GetMouseYImp() const = 0;

		virtual bool IsCursorHoveringImp() const = 0;
		virtual void SetCursorVisibilityImp(bool visible) const = 0;

	private:
		static Scope<Input> s_Instance;
	};
}