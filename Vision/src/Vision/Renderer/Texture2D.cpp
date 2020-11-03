#include "pch.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLTexture2D.h"
#endif

namespace Vision
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const TextureProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return CreateRef<OpenGLTexture2D>(width, height, props);
			}
			break;
		}
	}

	Ref<Texture2D> Texture2D::CreateFromFile(const std::string& path, const TextureProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return CreateRef<OpenGLTexture2D>(path, props);
			}
			break;
		}
	}
}