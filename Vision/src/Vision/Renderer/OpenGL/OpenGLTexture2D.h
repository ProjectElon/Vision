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
		
		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }
		inline const std::string& GetName() const override { return m_Name; }

	private:
		uint32_t m_RendererID;
		std::string m_Name;
		uint32_t m_Width;
		uint32_t m_Height;

		GLenum m_InternalFormat;
		GLenum m_TextureFormat;

	private:
		const static uint32_t s_WrapModeMap[];
		const static uint32_t s_FilterModeMap[];
	};
}