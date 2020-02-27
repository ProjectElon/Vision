#pragma once

#include "Vision/Renderer/Texture.h"

#include <glad/glad.h>

namespace Vision
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(uint32_t width, uint32_t height, const TextureProps& props);
		OpenGLTexture(const std::string& path, const TextureProps& props);
		
		~OpenGLTexture();

		void SetWrapOnX(WrapMode wrapMode) override;
		void SetWrapOnY(WrapMode wrapMode) override;
		void SetWrapBoth(WrapMode wrapModeX, WrapMode wrapModeY) override;
		void SetBorderColor(const glm::vec4& color) override;
		void SetFilterMode(FilterMode filterMode) override;

		void SetData(void* data, uint32_t sizeInBytes) override;

		void Bind(uint32_t slot) override;
		void UnBind() override;
		
		inline const TextureProps& GetProps() const override { return m_Props; }
		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }
		inline const std::string& GetName() const override { return m_Name; }
		
	private:
		uint32_t GetWrapModeOpenGL(WrapMode wrapMode);
		void ApplyFilter(FilterMode filterMode);

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;
		std::string m_Name;
		
		GLenum m_InternalFormat, m_TextureFormat;
		TextureProps m_Props;
	};
}