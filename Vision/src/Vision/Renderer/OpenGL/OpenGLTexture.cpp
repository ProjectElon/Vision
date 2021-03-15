#include "pch.h"
#include "Vision/Core/Common.h"
#include "Vision/Renderer/Texture.h"
#include "Vision/IO/FileSystem.h"

#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vision
{
	Texture::Texture(void* pixels, uint32 width, uint32 height, PixelFormat format)
	{
		Init(pixels, width, height, format);
	}

	Texture::~Texture()
	{
		Uninit();
	}

	void Texture::Init(void* pixels,
					   uint32 width,
					   uint32 height,
					   PixelFormat format)
	{
		Width  = width;
		Height = height;
		Format = format;

		GLenum internalFormat = GLInternalFormat(format);
		GLenum textureFormat  = GLTextureFormat(format);

		glCreateTextures(GL_TEXTURE_2D, 1, &RendererID);
		glTextureStorage2D(RendererID,
		                   1,
		                   internalFormat,
		                   Width,
		                   Height);

		glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GLFilterMode(Filter));

		glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GLWrapMode(WrapX));
		glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GLWrapMode(WrapY));

		glTextureSubImage2D(RendererID,
							0,
							0,
							0,
							Width,
							Height,
							textureFormat,
							GL_UNSIGNED_BYTE,
							pixels);
	}

	void Texture::Uninit()
	{
		glDeleteTextures(1, &RendererID);

		RendererID = 0;
		Width  = 0;
		Height = 0;
	}

	void Texture::Bind(uint32 textureSlot) const
	{
		glBindTextureUnit(textureSlot, RendererID);
	}

	void Texture::UnBind()
	{
		glBindTextures(GL_TEXTURE_2D, 1, 0);
	}

	void Texture::SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY)
	{
		WrapX = wrapModeX;
		WrapY = wrapModeY;

		glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GLWrapMode(wrapModeX));
		glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GLWrapMode(wrapModeY));
	}

	void Texture::SetFilterMode(FilterMode filter)
	{
		Filter = filter;
		glTextureParameteri(RendererID,
							GL_TEXTURE_MAG_FILTER,
							GLFilterMode(filter));
	}

	AssetLoadingData LoadTexture(const std::string& texturepath)
	{
		AssetLoadingData textureAsset;

		int32 width;
		int32 height;
		int32 channelCount;

		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pixels = stbi_load(texturepath.c_str(),
								    &width,
								    &height,
								    &channelCount,
								    0);

		if (pixels)
		{
			PixelFormat pixelFormat;

			if (channelCount == 1)
			{
				pixelFormat = PixelFormat::R;
			}
			else if (channelCount == 3)
			{
				pixelFormat = PixelFormat::RGB;
			}
			else if (channelCount == 4)
			{
				pixelFormat = PixelFormat::RGBA;
			}

			Texture* texture = new Texture(pixels, width, height, pixelFormat);

			textureAsset.Memory = texture;
			textureAsset.SizeInBytes = sizeof(Texture);
			textureAsset.TotalSizeInBytes = sizeof(Texture) + (width * height * channelCount);

			stbi_image_free(pixels);
		}

		return textureAsset;
	}

	void UnloadTexture(Asset* textureAsset)
	{
		Texture* texture = (Texture*)textureAsset->Memory;
		delete texture;
	}
}