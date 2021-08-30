#include "pch.hpp"
#include "Vision/Core/Defines.h"

#ifdef VN_RENDERER_API_OPENGL

#include "Vision/Renderer/OpenGL/OpenGLTexture.h"

#include "Vision/Platform/FileSystem.h"

#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

#include "Vision/Renderer/RendererTypes.h"
#include "Vision/Platform/Memory.h"

namespace Vision
{
	void OpenGLInitTexture(Texture* texture,
	                	   void* pixels,
					       uint32 width,
					       uint32 height,
					       PixelFormat format,
					       WrapMode wrapX,
                           WrapMode wrapY,
                           FilterMode filter)
	{
		texture->Width  = width;
		texture->Height = height;
		texture->Format = format;
		texture->WrapX  = wrapX;
		texture->WrapY  = wrapY;
		texture->Filter = filter;

		GLenum internalFormat = GLInternalFormat(format);
		GLenum textureFormat  = GLTextureFormat(format);

		auto& openglTexture = texture->OpenGL;

		glCreateTextures(GL_TEXTURE_2D, 1, &openglTexture.TextureHandle);
		glTextureStorage2D(openglTexture.TextureHandle,
		                   1,
		                   internalFormat,
		                   width,
		                   height);

		glTextureParameteri(openglTexture.TextureHandle, GL_TEXTURE_MIN_FILTER, GLFilterMode(texture->Filter));
		glTextureParameteri(openglTexture.TextureHandle, GL_TEXTURE_MAG_FILTER, GLFilterMode(texture->Filter));

		glTextureParameteri(openglTexture.TextureHandle, GL_TEXTURE_WRAP_S, GLWrapMode(texture->WrapX));
		glTextureParameteri(openglTexture.TextureHandle, GL_TEXTURE_WRAP_T, GLWrapMode(texture->WrapY));

		glTextureSubImage2D(openglTexture.TextureHandle,
							0,
							0,
							0,
							width,
							height,
							textureFormat,
							GL_UNSIGNED_BYTE,
							pixels);
	}

	void OpenGLUninitTexture(Texture* texture)
	{
		auto& openglTexture = texture->OpenGL;
		glDeleteTextures(1, &openglTexture.TextureHandle);

#ifndef VN_DIST
		openglTexture.TextureHandle = 0;
		texture->Width  = 0;
		texture->Height = 0;
#endif
	}

	void OpenGLBindTexture(Texture* texture, uint32 textureSlot)
	{
		auto& openglTexture = texture->OpenGL;
		glBindTextureUnit(textureSlot, openglTexture.TextureHandle);
	}

	void OpenGLSetTextureWrapMode(Texture* texture, WrapMode wrapModeX, WrapMode wrapModeY)
	{
		auto& openglTexture = texture->OpenGL;
		texture->WrapX = wrapModeX;
		texture->WrapY = wrapModeY;
		glTextureParameteri(openglTexture.TextureHandle, GL_TEXTURE_WRAP_S, GLWrapMode(wrapModeX));
		glTextureParameteri(openglTexture.TextureHandle, GL_TEXTURE_WRAP_T, GLWrapMode(wrapModeY));
	}

	void OpenGLSetTextureFilterMode(Texture* texture, FilterMode filter)
	{
		auto& openglTexture = texture->OpenGL;
		texture->Filter = filter;
		glTextureParameteri(openglTexture.TextureHandle,
							GL_TEXTURE_MAG_FILTER,
							GLFilterMode(filter));
	}
}

#endif