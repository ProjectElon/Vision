#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLTexture.h"
#include "Vision/Core/Core.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vision
{
	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, const TextureProps& props)
		: m_RendererID(0)
		, m_Width(width)
		, m_Height(height)
		, m_Props(props)
	{
		m_InternalFormat = GL_RGBA8;
		m_TextureFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapModeOpenGL(props.WrapX));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapModeOpenGL(props.WrapY));
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &props.BorderColor.r);

		ApplyFilter(props.Filter);
	}

	OpenGLTexture::OpenGLTexture(const std::string& path, const TextureProps& props)
		: m_Props(props)
	{
		size_t lastSlash = path.find_last_of("/\\");
		m_Name = path.substr(lastSlash + 1);
		
		int width, height;
		int channelCount;

		stbi_set_flip_vertically_on_load(true);
		uint8_t* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

		if (data)
		{
			m_Width = width;
			m_Height = height;
			
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

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_TextureFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapModeOpenGL(props.WrapX));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapModeOpenGL(props.WrapY));
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &props.BorderColor.r);
			ApplyFilter(props.Filter);

			stbi_image_free(data);
			
			VN_CORE_TRACE("[TEXTURE]({0}) Loaded Successfully", m_Name);
		}
		else
		{
			VN_CORE_ERROR("[TEXTURE]({0}) Failed To Load", m_Name);
		}
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture::SetWrapOnX(WrapMode wrapMode) 
	{
		m_Props.WrapX = wrapMode;

		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapModeOpenGL(wrapMode));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::SetWrapOnY(WrapMode wrapMode) 
	{
		m_Props.WrapY = wrapMode;
		
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapModeOpenGL(wrapMode));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::SetWrapBoth(WrapMode wrapModeX, WrapMode wrapModeY) 
	{
		m_Props.WrapX = wrapModeX;
		m_Props.WrapY = wrapModeY;

		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapModeOpenGL(wrapModeX));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapModeOpenGL(wrapModeY));
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::SetBorderColor(const glm::vec4& color) 
	{
		m_Props.BorderColor = color;
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color.r);
	}

	void OpenGLTexture::SetFilterMode(FilterMode filterMode) 
	{
		m_Props.Filter = filterMode;

		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		ApplyFilter(filterMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::SetData(void* data, uint32_t sizeInBytes)
	{
		uint32_t bytesPerPixel = (m_TextureFormat == GL_RGBA) ? 4 : 3;
		VN_CORE_ASSERT(bytesPerPixel * m_Width * m_Height == sizeInBytes, "data must have the same size as the texture");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_TextureFormat, GL_UNSIGNED_BYTE, data);
	}

	uint32_t OpenGLTexture::GetWrapModeOpenGL(WrapMode wrapMode)
	{
		switch (wrapMode)
		{
			case WrapMode::Repeat:
			{
				return GL_REPEAT;
			}
			break;

			case WrapMode::MirroredRepeat:
			{
				return GL_MIRRORED_REPEAT;
			}
			break;

			case WrapMode::ClampToEdge:
			{
				return GL_CLAMP_TO_EDGE;
			}
			break;

			case WrapMode::ClampToBorder:
			{
				return GL_CLAMP_TO_BORDER;
			}
			break;
		}
	}

	void OpenGLTexture::ApplyFilter(FilterMode filterMode)
	{
		switch (filterMode)
		{
			case FilterMode::Point:
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			break;

			case FilterMode::Bilinear:
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			break;
		}
	}
}