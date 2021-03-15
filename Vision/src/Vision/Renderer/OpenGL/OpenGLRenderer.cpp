#include "pch.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Platform/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Vision
{

#ifdef VN_DEBUG
	static void APIENTRY OpenGLDebugOutput(GLenum source,
										   GLenum type,
										   GLuint id,
										   GLenum severity,
										   GLsizei length,
										   const GLchar* message,
										   const void* userParam);
#endif

	void Renderer::Init(Window* renderTargetWindow)
	{
		RenderTargetWindow = renderTargetWindow;

#if VN_PLATFORM_MACOS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		GLFWwindow* WindowHandle = (GLFWwindow*)renderTargetWindow->Handle;
		glfwMakeContextCurrent(WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VnCoreAssert(status, "Unable to initialize glad");

		VnCoreInfo("GPU Vendor : {0}", glGetString(GL_VENDOR));
		VnCoreInfo("Graphics Device : {0}", glGetString(GL_RENDERER));
		VnCoreInfo("OpenGL Version : {0}", glGetString(GL_VERSION));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef VN_DEBUG

		int32 flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE,
								  GL_DONT_CARE,
								  GL_DONT_CARE,
								  0,
								  nullptr,
								  GL_TRUE);
		}

#endif
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::SetViewport(uint32 x,
							   uint32 y,
							   uint32 width,
							   uint32 height)
	{
		ViewportWidth = width;
		ViewportHeight = height;
		glViewport(x, y, width, height);
	}

	void Renderer::SetClearColor(float32 r, float32 g, float32 b, float32 a)
	{
		glClearColor(r, g, b, a);
	}

	void Renderer::Clear(ClearFlags flags)
	{
		uint32 glClearFlags = 0;

		if (flags & ClearColorBuffer)
		{
			glClearFlags |= GL_COLOR_BUFFER_BIT;
		}

		if (flags & ClearDepthBuffer)
		{
			glClearFlags |= GL_DEPTH_BUFFER_BIT;
		}

		if (flags & ClearStencilBuffer)
		{
			glClearFlags |= GL_STENCIL_BUFFER_BIT;
		}

		glClear(glClearFlags);
	}

	void Renderer::DrawIndexed(const VertexBuffer* vertexBuffer,
							   const IndexBuffer* indexBuffer,
							   uint32 count,
							   Primitive primitive)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)0);
	}

	int32 Renderer::GetMaxTextureSlots()
	{
		int32 textureSlots;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);
		return textureSlots;
	}

	void Renderer::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
	}

	void Renderer::SwapBuffers()
	{
		glfwSwapBuffers((GLFWwindow*)RenderTargetWindow->Handle);
	}

#ifdef VN_DEBUG

	static void APIENTRY OpenGLDebugOutput(GLenum source,
										   GLenum type,
										   GLuint id,
										   GLenum severity,
										   GLsizei length,
										   const GLchar* message,
										   const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:			VnCoreCritical(message);  break;
		 	case GL_DEBUG_SEVERITY_MEDIUM:			VnCoreError(message);     break;
			// case GL_DEBUG_SEVERITY_LOW:			VnCoreWarn(message);      break;
			// case GL_DEBUG_SEVERITY_NOTIFICATION: VnCoreTrace(message);     break;
		}
	}

	Window* Renderer::RenderTargetWindow;
	uint32 Renderer::ViewportWidth;
	uint32 Renderer::ViewportHeight;

#endif
}