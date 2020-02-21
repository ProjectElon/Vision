#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLTexture.h"
#include "Vision/Core/Core.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vision
{
	OpenGLTexture::OpenGLTexture(glm::vec4* colors, uint32_t width, uint32_t height, const TextureProps& props)
		: m_Props(props)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapModeOpenGL(props.WrapX));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapModeOpenGL(props.WrapY));

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &props.BorderColor.r);

		ApplyFilter(props.Filter);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, colors);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	OpenGLTexture::OpenGLTexture(const std::string& path, const TextureProps& props)
		: m_Props(props)
	{
		size_t lastSlash = path.find_last_of('/');
		std::string name = path.substr(lastSlash + 1);

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapModeOpenGL(props.WrapX));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapModeOpenGL(props.WrapY));

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &props.BorderColor.r);

		ApplyFilter(props.Filter);

		int width, height;
		int channelCount;

		stbi_set_flip_vertically_on_load(true);
		uint8_t* imageData = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

		if (imageData)
		{
			m_Width = width;
			m_Height = height;
			
			uint32_t format = 0;

			if (channelCount == 1)
			{
				format = GL_RED;
			}
			else if (channelCount == 3)
			{
				format = GL_RGB;
			}
			else if (channelCount == 4)
			{
				format = GL_RGBA;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(imageData);
			
			VN_CORE_TRACE("[TEXTURE]({0}) : Loaded Successfully", name);
		}
		else
		{
			VN_CORE_ERROR("[TEXTURE]({0}) : Failed To Load", name);
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
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
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