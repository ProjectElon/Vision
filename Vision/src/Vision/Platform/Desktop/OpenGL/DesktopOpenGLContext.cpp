#include "pch.h"
#include "DesktopOpenGLContext.h"
#include "Vision/Core/Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Vision
{
	DesktopOpenGLContext::DesktopOpenGLContext(Window* window)
		: GraphicsContext(window)
	{
		// making sure we atleast have OpenGL version 4.5 and core profile
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_WindowHandle = (GLFWwindow*)window->GetNativeWindowHandle();
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VN_CORE_ASSERT(status, "Unable to initialize glad (OpenGL)");

		VN_CORE_INFO("GPU Vendor : {0}", glGetString(GL_VENDOR));
		VN_CORE_INFO("Graphics Device : {0}", glGetString(GL_RENDERER));
		VN_CORE_INFO("OpenGL version : {0}", glGetString(GL_VERSION));
	}
	
	void DesktopOpenGLContext::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
	}

	void DesktopOpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}