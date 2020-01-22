#include "pch.h"
#include "DesktopInput.h"
#include "Vision/Core/Application.h"

namespace Vision
{
	DesktopInput::DesktopInput()
	{
		Application& app = Application::Get();
		m_Window = (GLFWwindow*)app.GetWindow().GetNativeWindow();
	}

	bool DesktopInput::IsKeyPressedImp(unsigned int keyCode)
	{
		int state = glfwGetKey(m_Window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool DesktopInput::IsKeyReleasedImp(unsigned int keyCode)
	{
		return glfwGetKey(m_Window, keyCode) == GLFW_RELEASE;
	}

	bool DesktopInput::IsMouseButtonPressedImp(unsigned int button)
	{
		return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
	}

	bool DesktopInput::IsMouseButtonReleasedImp(unsigned int button)
	{
		return glfwGetMouseButton(m_Window, button) == GLFW_RELEASE;
	}

	std::pair<float, float> DesktopInput::GetMousePositionImp()
	{
		double xpos, ypos;
		glfwGetCursorPos(m_Window, &xpos, &ypos);

		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	float DesktopInput::GetMouseXImp()
	{
		auto [xpos, ypos] = GetMousePositionImp();
		return xpos;
	}

	float DesktopInput::GetMouseYImp()
	{
		auto [xpos, ypos] = GetMousePositionImp();
		return ypos;
	}
}
