#include "pch.h"
#include "Vision/Renderer/Texture.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLTexture.h"
#endif

namespace Vision
{
	Ref<Texture> Texture::Create(glm::vec4* colors, uint32_t width, uint32_t height, const TextureProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return MakeRef<OpenGLTexture>(colors, width, height, props);
			}
			break;
		}
	}

	Ref<Texture> Texture::CreateFromFile(const std::string& path, const TextureProps& props)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return MakeRef<OpenGLTexture>(path, props);
			}
			break;
		}
	}
}