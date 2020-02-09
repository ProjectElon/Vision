#include "pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Vision
{
	void OpenGLRendererAPI::OnWindowResize(uint32_t width, uint32_t height) const
	{
		glViewport(0, 0, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) const 
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear(uint32_t flags) const
	{
		uint32_t glFlags = 0;
		
		if (flags & ClearColorBuffer)
		{
			glFlags |= GL_COLOR_BUFFER_BIT;
		}

		if (flags & ClearDepthBuffer)
		{
			glFlags |= GL_DEPTH_BUFFER_BIT;
		}

		if (flags & ClearStencilBuffer)
		{
			glFlags |= GL_STENCIL_BUFFER_BIT;
		}

		glClear(glFlags);
	}

	void OpenGLRendererAPI::DrawIndexed(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer) const
	{
		vertexBuffer.Bind();
		indexBuffer.Bind();

		glDrawElements(GL_TRIANGLES, indexBuffer.GetIndexCount(), GL_UNSIGNED_INT, (const void*)0);
	}
}