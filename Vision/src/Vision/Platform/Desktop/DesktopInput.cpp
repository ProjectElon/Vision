#include "pch.h"

#include "Vision/Platform/Input.h"
#include "Vision/Core/Application.h"

#ifdef VN_PLATFORM_DESKTOP

#include <GLFW/glfw3.h>

namespace Vision
{
	static GLFWwindow* window;

	void Input::Init()
	{
		Application& app = Application::Get();
		window = (GLFWwindow*)app.GetWindow().GetNativeHandle();
	}

	void Input::Shutdown()
	{
	}

	bool Input::IsKeyDown(uint32 keyCode)
	{
		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyUp(uint32 keyCode)
	{
		return glfwGetKey(window, keyCode) == GLFW_RELEASE;
	}

	bool Input::IsMouseButtonDown(uint32 button)
	{
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	bool Input::IsMouseButtonUp(uint32 button)
	{
		return glfwGetMouseButton(window, button) == GLFW_RELEASE;
	}

	glm::vec2 Input::GetMousePosition()
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { (float)x, (float)y };
	}

	bool Input::IsCursorHovering()
	{
		return glfwGetWindowAttrib(window, GLFW_HOVERED);
	}

	void Input::SetCursorVisibility(bool visible)
	{
		if (visible)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			
			if (glfwRawMouseMotionSupported())
			{
				glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			}
		}
	}
}

#endif