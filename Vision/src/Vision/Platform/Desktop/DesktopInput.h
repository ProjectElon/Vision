#pragma once

#include "Vision/Platform/Input.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	class DesktopInput : public Input
	{
	public:
		DesktopInput();

		bool IsKeyDownImp(unsigned int keyCode) const override;
		bool IsKeyUpImp(unsigned int keyCode) const override;
		
		bool IsMouseButtonDownImp(unsigned int button) const override;
		bool IsMouseButtonUpImp(unsigned int button) const override;
		std::pair<float, float> GetMousePositionImp() const override;
		float GetMouseXImp() const override;
		float GetMouseYImp() const override;

		bool IsCursorHoveringImp() const override;
		void SetCursorVisibilityImp(bool visible) const override;

	private:
		GLFWwindow* m_Window;
	};
}
