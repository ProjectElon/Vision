#pragma once

#include "Vision/Platform/Input.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	class DesktopInput : public Input
	{
	public:
		DesktopInput();

	protected:
		virtual bool IsKeyPressedImp(unsigned int keyCode) override;
		virtual bool IsKeyReleasedImp(unsigned int keyCode) override;
		virtual bool IsMouseButtonPressedImp(unsigned int button) override;
		virtual bool IsMouseButtonReleasedImp(unsigned int button) override;
		virtual std::pair<float, float> GetMousePositionImp() override;
		virtual float GetMouseXImp() override;
		virtual float GetMouseYImp() override;

	private:
		GLFWwindow* m_Window;
	};
}
