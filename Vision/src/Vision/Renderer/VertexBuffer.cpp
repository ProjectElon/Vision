#include "pch.h"
#include "Vision/Renderer/VertexBuffer.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLVertexBuffer.h"
#endif 

namespace Vision
{
	Ref<VertexBuffer> VertexBuffer::Create(const BufferProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return CreateRef<OpenGLVertexBuffer>(props);
			}
			break;
		}
	}
}