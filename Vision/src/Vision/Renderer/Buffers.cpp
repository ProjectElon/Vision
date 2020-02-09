#include "pch.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLBuffers.h"
#endif

namespace Vision
{
	VertexBuffer* VertexBuffer::Create(const BufferProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return new OpenGLVertexBuffer(props);
			}
			break;
		}
	}

	IndexBuffer* IndexBuffer::Create(const BufferProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return new OpenGLIndexBuffer(props);
			}
			break;
		}
	}
}