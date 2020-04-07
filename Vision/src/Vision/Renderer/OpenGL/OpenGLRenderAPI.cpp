#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLRendererAPI.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"
#include "Vision/Renderer/VertexBuffer.h"
#include "Vision/Renderer/IndexBuffer.h"

#include <glad/glad.h>

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

	OpenGLRendererAPI::OpenGLRendererAPI()
	{
		// To Be Abstracted Later

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef VN_DEBUG

		int32_t flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

#endif
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) const 
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear(uint32_t flags) const
	{
		uint32_t glClearFlags = 0;

		if (flags & API::ClearColorBufferBit)
		{
			glClearFlags |= GL_COLOR_BUFFER_BIT;
		}
		
		if (flags & API::ClearDepthBufferBit)
		{
			glClearFlags |= GL_DEPTH_BUFFER_BIT;
		}

		if (flags & API::ClearStencilBufferBit)
		{
			glClearFlags |= GL_STENCIL_BUFFER_BIT;
		}
		
		glClear(glClearFlags);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t count) const
	{
		glDrawElements(GL_TRIANGLES, count, s_TypeMap[(int)indexBuffer->GetDataType()], (const void*)0);
	}

	int32_t OpenGLRendererAPI::GetMaxTextureSlots() const
	{
		int32_t textureSlots;
		glGetIntegerv(GL_MAX_IMAGE_UNITS, &textureSlots);
		return textureSlots;
	}

	uint32_t OpenGLRendererAPI::s_TypeMap[] =
	{
		GL_BOOL,
		GL_BYTE,
		GL_UNSIGNED_BYTE,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT,
		GL_FLOAT
	};

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
			case GL_DEBUG_SEVERITY_HIGH:		 VN_CORE_CRITICAL(message); break;
			case GL_DEBUG_SEVERITY_MEDIUM:		 VN_CORE_ERROR(message);    break;
			case GL_DEBUG_SEVERITY_LOW:			 VN_CORE_WARN(message);     break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: VN_CORE_TRACE(message);    break;
		}
	}

#endif
}