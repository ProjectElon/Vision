#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Application.h"
#include "Vision/Core/Logger.h"

#ifdef VN_RENDERER_API_OPENGL

#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/OpenGL/OpenGLRenderer.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Platform/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace Vision
{
	static void APIENTRY OpenGLDebugOutput(GLenum source,
										   GLenum type,
										   GLuint id,
										   GLenum severity,
										   GLsizei length,
										   const GLchar* message,
										   const void* userParam);

	void InitializeOpenGL(RendererAPI* rendererAPI)
	{
#if VN_PLATFORM_MACOS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		Window* window = &Application::Get().Window;
		glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->Handle));

		int32 status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VnCoreAssert(status, "failed to initialize glad");

		VnCoreInfo("GPU Vendor : {0}",      glGetString(GL_VENDOR));
		VnCoreInfo("Graphics Device : {0}", glGetString(GL_RENDERER));
		VnCoreInfo("OpenGL Version : {0}",  glGetString(GL_VERSION));

		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Renderer::Capabilites.MaxTextureSlots);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Pipeline State Functions
		rendererAPI->SetClearColor = &OpenGLSetClearColor;
		rendererAPI->Clear 		   = &OpenGLClear;
		rendererAPI->SetViewport   = &OpenGLSetViewport;
		rendererAPI->DrawIndexed16 = &OpenGLDrawIndexed16;
		rendererAPI->DrawIndexed32 = &OpenGLDrawIndexed32;
		rendererAPI->SetVSync	   = &OpenGLSetVSync;
		rendererAPI->SwapBuffers   = &OpenGLSwapBuffers;

		// Buffers Functions
		rendererAPI->InitVertexBuffer      = &OpenGLInitVertexBuffer;
		rendererAPI->UninitVertexBuffer    = &OpenGLUninitVertexBuffer;
		rendererAPI->SetVertexBufferLayout = &OpenGLSetVertexBufferLayout;
		rendererAPI->SetVertexBufferData   = &OpenGLSetVertexBufferData;
		rendererAPI->BindVertexBuffer 	   = &OpenGLBindVertexBuffer;
		rendererAPI->InitIndexBuffer16 	   = &OpenGLInitIndexBuffer16;
		rendererAPI->InitIndexBuffer32 	   = &OpenGLInitIndexBuffer32;
		rendererAPI->BindIndexBuffer   	   = &OpenGLBindIndexBuffer;

		// Texture Functions
		rendererAPI->InitTexture   	      = &OpenGLInitTexture;
		rendererAPI->UninitTexture 	      = &OpenGLUninitTexture;
		rendererAPI->BindTexture   	      = &OpenGLBindTexture;
		rendererAPI->SetTextureWrapMode   = &OpenGLSetTextureWrapMode;
		rendererAPI->SetTextureFilterMode = &OpenGLSetTextureFilterMode;

		// Shader Functions
		rendererAPI->InitShader         = &OpenGLInitShader;
		rendererAPI->UninitShader       = &OpenGLUninitShader;
		rendererAPI->BindShader         = &OpenGLBindShader;
		rendererAPI->SetUniformInt 	    = &OpenGLSetUniformInt;
		rendererAPI->SetUniformIntArray = &OpenGLSetUniformIntArray;
		rendererAPI->SetUniformFloat    = &OpenGLSetUniformFloat;
		rendererAPI->SetUniformFloat2   = &OpenGLSetUniformFloat2;
		rendererAPI->SetUniformFloat3   = &OpenGLSetUniformFloat3;
		rendererAPI->SetUniformFloat4   = &OpenGLSetUniformFloat4;
		rendererAPI->SetUniformMatrix3  = &OpenGLSetUniformMatrix3;
		rendererAPI->SetUniformMatrix4  = &OpenGLSetUniformMatrix4;

		// FrameBuffer Functions
		rendererAPI->InitFrameBuffer      = &OpenGLInitFrameBuffer;
		rendererAPI->UninitFrameBuffer    = &OpenGLUninitFrameBuffer;
		rendererAPI->ResizeFrameBuffer    = &OpenGLResizeFrameBuffer;
		rendererAPI->ReadPixel 		      = &OpenGLReadPixel;
		rendererAPI->ClearColorAttachment = &OpenGLClearColorAttachment;
		rendererAPI->BindFrameBuffer      = &OpenGLBindFrameBuffer;
		rendererAPI->UnbindFrameBuffer    = &OpenGLUnbindFrameBuffer;

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

	void UninitializeOpenGL()
	{
	}

	void OpenGLSetClearColor(float32 r, float32 g, float32 b, float32 a)
	{
		glClearColor(r, g, b, a);
	}

	void OpenGLClear(uint32 flags)
	{
		// @Optimization: check if the compiler optimizes the branch version better
		glClear((GL_COLOR_BUFFER_BIT   * (flags & ClearFlags_Color)) |
		        (GL_DEPTH_BUFFER_BIT   * (flags & ClearFlags_Depth)) |
		        (GL_STENCIL_BUFFER_BIT * (flags & ClearFlags_Stencil)));
	}

	void OpenGLSetViewport(const ViewportRect* viewportRect)
	{
		Renderer::ViewportRect = *viewportRect;
		glViewport(viewportRect->X, viewportRect->Y, viewportRect->Width, viewportRect->Height);
	}

	void OpenGLDrawIndexed16(const VertexBuffer* vertexBuffer,
							 const IndexBuffer*  indexBuffer,
							 uint32 count)
	{
		glDrawElements(GL_TRIANGLES,
		               count,
		               GL_UNSIGNED_SHORT,
		               static_cast<const void*>(0));
	}

	void OpenGLDrawIndexed32(const VertexBuffer* vertexBuffer,
							 const IndexBuffer*  indexBuffer,
							 uint32 count)
	{
		glDrawElements(GL_TRIANGLES,
		               count,
		               GL_UNSIGNED_INT,
		               static_cast<const void*>(0));
	}

	void OpenGLSetVSync(bool enabled)
	{
		glfwSwapInterval(enabled);
	}

	void OpenGLSwapBuffers()
	{
		Window* window = &Application::Get().Window;
		glfwSwapBuffers(static_cast<GLFWwindow*>(window->Handle));
	}

	void APIENTRY OpenGLDebugOutput(GLenum source,
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
}

#endif