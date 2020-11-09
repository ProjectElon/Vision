#include "pch.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLTexture2D.h"
#endif

namespace Vision
{
	Texture2D* Texture2D::Create(uint32 width, uint32 height, const TextureProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return new OpenGLTexture2D(width, height, props);
			}
			break;
		}

		return nullptr;
	}

	Texture2D* Texture2D::CreateFromFile(const std::string& path, const TextureProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return new OpenGLTexture2D(path, props);
			}
			break;
		}

		return nullptr;
	}
}