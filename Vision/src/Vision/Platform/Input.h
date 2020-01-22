#pragma once

#include "pch.h"

namespace Vision
{
	class Input
	{
	public:
		static void Initialize();
		static void ShutDown();

		inline static bool IsKeyPressed(unsigned int keyCode) { return s_Instance->IsKeyPressedImp(keyCode); }
		inline static bool IsKeyReleased(unsigned int keyCode) { return s_Instance->IsKeyReleasedImp(keyCode); }
		inline static bool IsMouseButtonPressed(unsigned int button) { return s_Instance->IsMouseButtonPressed(button); }
		inline static bool IsMouseButtonReleased(unsigned int button) { return s_Instance->IsMouseButtonReleased(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImp(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImp(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImp(); }

	protected:
		virtual bool IsKeyPressedImp(unsigned int keyCode) = 0;
		virtual bool IsKeyReleasedImp(unsigned int keyCode) = 0;
		virtual bool IsMouseButtonPressedImp(unsigned int button) = 0;
		virtual bool IsMouseButtonReleasedImp(unsigned int button) = 0;
		virtual std::pair<float, float> GetMousePositionImp() = 0;
		virtual float GetMouseXImp() = 0;
		virtual float GetMouseYImp() = 0;

	private:
		static Input* s_Instance;
	};
}