#pragma once

#include "Vision/Renderer/Texture2D.h"

#include <glad/glad.h>

namespace Vision
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32 width, uint32 height, const TextureProps& props);
		OpenGLTexture2D(const std::string& filepath, const TextureProps& props);

		~OpenGLTexture2D();

		void SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY) override;
		void SetFilterMode(FilterMode filterMode) override;

		void SetData(void* data, uint32 sizeInBytes) override;

		void Bind(uint32 slot) const override;

	private:
		GLenum m_InternalFormat;
		GLenum m_TextureFormat;

	private:
		const static uint32 s_WrapModeMap[];
		const static uint32 s_FilterModeMap[];
	};
}