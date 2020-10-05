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

	const Ref<Texture2D>& Texture2D::GetDefault()
	{
		static Ref<Texture2D> texture;

		if (!texture)
		{
			// Note: creating a single colored texture depends 
			// on the endianness of the processor

			bool isLittle = false;
			
			{
				uint32 x = 1;
				isLittle =  *((uint8*)(&x)) == 1;
			}

			uint32 pink = isLittle ? 0xFFFF00FF : 0xFF00FFFF;

			texture = Texture2D::Create(1, 1);
			texture->SetData(&pink, sizeof(uint32) * 1 * 1);
		}

		return texture;
	}
}