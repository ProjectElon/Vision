#include "pch.h"
#include "Vision/Renderer/IndexBuffer.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLIndexBuffer.h"
#endif

namespace Vision
{
	Ref<IndexBuffer> IndexBuffer::Create(const BufferProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return CreateRef<OpenGLIndexBuffer>(props);
			}
			break;
		}
	}
}