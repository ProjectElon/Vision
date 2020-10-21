#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLTexture2D.h"
#include "Vision/Core/Base.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vision
{
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureProps& props)
		: m_InternalFormat(GL_RGBA8)
		, m_TextureFormat(GL_RGBA)
	{
		m_Data.Width  = width;
		m_Data.Height = height;
		m_Properties  = props;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_Data.RendererID);
		glTextureStorage2D(m_Data.RendererID, 1, m_InternalFormat, m_Data.Width, m_Data.Height);

		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_MAG_FILTER, s_FilterModeMap[(uint32)props.FilterMode]);

		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_WRAP_S, s_WrapModeMap[(uint32)props.WrapX]);
		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_WRAP_T, s_WrapModeMap[(uint32)props.WrapY]);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, const TextureProps& props)
	{
		size_t lastSlash = filepath.find_last_of("/\\");
		size_t lastDot = filepath.rfind('.');

		size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
		size_t count = (lastDot == std::string::npos) ? filepath.length() - start : lastDot - start;

		m_Properties = props;
		m_Data.Name = filepath.substr(start, count);

		int width, height;
		int channelCount;

		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(filepath.c_str(), &width, &height, &channelCount, 0);

		if (data)
		{
			m_Data.Width  = width;
			m_Data.Height = height;

			if (channelCount == 1)
			{
				m_InternalFormat = GL_R8;
				m_TextureFormat = GL_RED;
			}
			else if (channelCount == 3)
			{
				m_InternalFormat = GL_RGB8;
				m_TextureFormat = GL_RGB;
			}
			else if (channelCount == 4)
			{
				m_InternalFormat = GL_RGBA8;
				m_TextureFormat = GL_RGBA;
			}

			VN_CORE_ASSERT(m_InternalFormat & m_TextureFormat, "UnSupported Format");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_Data.RendererID);
			glTextureStorage2D(m_Data.RendererID, 1, m_InternalFormat, m_Data.Width, m_Data.Height);

			glTextureParameteri(m_Data.RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_Data.RendererID, GL_TEXTURE_MAG_FILTER, s_FilterModeMap[(int)props.FilterMode]);

			glTextureParameteri(m_Data.RendererID, GL_TEXTURE_WRAP_S, s_WrapModeMap[(int)props.WrapX]);
			glTextureParameteri(m_Data.RendererID, GL_TEXTURE_WRAP_T, s_WrapModeMap[(int)props.WrapY]);

			glTextureSubImage2D(m_Data.RendererID, 0, 0, 0, m_Data.Width, m_Data.Height, m_TextureFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);

 			VN_CORE_INFO("[TEXTURE]({0}) ... Loaded Successfully", m_Data.Name);
		}
		else
		{
			VN_CORE_ERROR("[TEXTURE]({0}) Failed To Load", m_Data.Name);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_Data.RendererID);
	}

	void OpenGLTexture2D::SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY)
	{
		m_Properties.WrapX = wrapModeX;
		m_Properties.WrapY = wrapModeY;

		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_WRAP_S, s_WrapModeMap[(int)wrapModeX]);
		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_WRAP_T, s_WrapModeMap[(int)wrapModeY]);
	}

	void OpenGLTexture2D::SetFilterMode(FilterMode filterMode)
	{
		m_Properties.FilterMode = filterMode;
		glTextureParameteri(m_Data.RendererID, GL_TEXTURE_MAG_FILTER, s_FilterModeMap[(int)filterMode]);
	}

	void OpenGLTexture2D::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_Data.RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t sizeInBytes)
	{
		uint32_t bytesPerPixel = (m_TextureFormat == GL_RGBA) ? 4 : 3;
		VN_CORE_ASSERT(bytesPerPixel * m_Data.Width * m_Data.Height == sizeInBytes, "data must have the same size as the texture");
		glTextureSubImage2D(m_Data.RendererID, 0, 0, 0, m_Data.Width, m_Data.Height, m_TextureFormat, GL_UNSIGNED_BYTE, data);
	}

	const uint32_t OpenGLTexture2D::s_WrapModeMap[2] =
	{
		GL_REPEAT,
		GL_CLAMP_TO_EDGE
	};

	const uint32_t OpenGLTexture2D::s_FilterModeMap[2] =
	{
		GL_NEAREST,
		GL_LINEAR
	};
}