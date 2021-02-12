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
	void InitTexture(Texture* texture,
					   void* data,
					   uint32 width,
					   uint32 height,
					   PixelFormat pixelFormat)
	{
		texture->Width  = width;
		texture->Height = height;

		GLenum internalFormat = GLInternalFormat(pixelFormat);
		GLenum textureFormat  = GLTextureFormat(pixelFormat);

		glCreateTextures(GL_TEXTURE_2D, 1, &texture->RendererID);
		glTextureStorage2D(texture->RendererID,
		                   1,
		                   internalFormat,
		                   texture->Width,
		                   texture->Height);

		glTextureParameteri(texture->RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(texture->RendererID, GL_TEXTURE_MAG_FILTER, GLFilterMode(texture->FilterMode));

		glTextureParameteri(texture->RendererID, GL_TEXTURE_WRAP_S, GLWrapMode(texture->WrapX));
		glTextureParameteri(texture->RendererID, GL_TEXTURE_WRAP_T, GLWrapMode(texture->WrapY));

		glTextureSubImage2D(texture->RendererID,
							0,
							0,
							0,
							texture->Width,
							texture->Height,
							textureFormat,
							GL_UNSIGNED_BYTE,
							data);
	}

	AssetLoadingData LoadTexture(const std::string& texturepath)
	{
		AssetLoadingData textureAsset;

		int32 width;
		int32 height;
		int32 channelCount;

		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(texturepath.c_str(),
								  &width,
								  &height,
								  &channelCount,
								  0);

		if (data)
		{
			Texture* texture = new Texture;

			texture->Width = width;
			texture->Height = height;

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

			InitTexture(texture,
						  data,
						  width,
						  height,
						  pixelFormat);

			textureAsset.Memory = texture;
			textureAsset.SizeInBytes = sizeof(Texture);
			textureAsset.TotalSizeInBytes = sizeof(Texture) + (width * height * channelCount);

			stbi_image_free(data);
		}

		return textureAsset;
	}

	void UnloadTexture(Asset* textureAsset)
	{
		Texture* texture = (Texture*)textureAsset->Memory;
		UninitTexture(texture);
		delete texture;
	}

	void UninitTexture(Texture* texture)
	{
		glDeleteTextures(1, &texture->RendererID);

		// @(Harlequin): not sure if 0 is invalid in opnegl land or not
		texture->RendererID = 0;
		texture->Width  = 0;
		texture->Height = 0;
	}

	void SetTextureWrapMode(Texture* texture, WrapMode wrapModeX, WrapMode wrapModeY)
	{
		texture->WrapX = wrapModeX;
		texture->WrapY = wrapModeY;

		glTextureParameteri(texture->RendererID,
							GL_TEXTURE_WRAP_S,
							GLWrapMode(wrapModeX));

		glTextureParameteri(texture->RendererID,
							GL_TEXTURE_WRAP_T,
							GLWrapMode(wrapModeY));
	}

	void SetTextureFilterMode(Texture* texture, FilterMode filterMode)
	{
		texture->FilterMode = filterMode;
		glTextureParameteri(texture->RendererID,
							GL_TEXTURE_MAG_FILTER,
							GLFilterMode(filterMode));
	}

	void BindTexture(const Texture* texture, uint32 textureSlot)
	{
		glBindTextureUnit(textureSlot, texture->RendererID);
	}

	void UnBindTexture(const Texture* texture)
	{
		glBindTextures(GL_TEXTURE_2D, 1, 0);
	}
}