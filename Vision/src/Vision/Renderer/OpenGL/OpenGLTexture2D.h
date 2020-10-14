#pragma once

#include "Vision/Renderer/Texture2D.h"

#include <glad/glad.h>

namespace Vision
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height, const TextureProps& props);
		OpenGLTexture2D(const std::string& filepath, const TextureProps& props);
		
		~OpenGLTexture2D();

		void SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY) override;
		void SetFilterMode(FilterMode filterMode) override;

		void SetData(void* data, uint32_t sizeInBytes) override;

		void Bind(uint32_t slot) override;

	private:
		GLenum m_InternalFormat;
		GLenum m_TextureFormat;

	private:
		const static uint32_t s_WrapModeMap[];
		const static uint32_t s_FilterModeMap[];
	};
}