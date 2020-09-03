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
		
	#if VN_PLATFORM_MACOS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

		m_WindowHandle = (GLFWwindow*)window->GetNativeHandle();
		glfwMakeContextCurrent(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VN_CORE_ASSERT(status, "Unable to initialize glad");

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