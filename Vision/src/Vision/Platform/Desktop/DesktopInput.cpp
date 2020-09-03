#include "pch.h"
#include "DesktopInput.h"
#include "Vision/Core/Application.h"

namespace Vision
{
	DesktopInput::DesktopInput()
	{
		m_Window = (GLFWwindow*)Application::Get().GetWindow().GetNativeHandle();
	}

	bool DesktopInput::IsKeyDownImp(unsigned int keyCode) const
	{
		int state = glfwGetKey(m_Window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool DesktopInput::IsKeyUpImp(unsigned int keyCode) const
	{
		return glfwGetKey(m_Window, keyCode) == GLFW_RELEASE;
	}

	bool DesktopInput::IsMouseButtonDownImp(unsigned int button) const
	{
		return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
	}

	bool DesktopInput::IsMouseButtonUpImp(unsigned int button) const
	{
		return glfwGetMouseButton(m_Window, button) == GLFW_RELEASE;
	}

	std::pair<float, float> DesktopInput::GetMousePositionImp() const
	{
		double xpos, ypos;
		glfwGetCursorPos(m_Window, &xpos, &ypos);

		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	float DesktopInput::GetMouseXImp() const
	{
		auto [xpos, ypos] = GetMousePositionImp();
		return xpos;
	}

	float DesktopInput::GetMouseYImp() const
	{
		auto [xpos, ypos] = GetMousePositionImp();
		return ypos;
	}

	bool DesktopInput::IsCursorHoveringImp() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_HOVERED);
	}

	void DesktopInput::SetCursorVisibilityImp(bool visible) const
	{
		if (visible)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
		else
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			
			if (glfwRawMouseMotionSupported())
			{
				glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			}
		}
	}
}
