#pragma once

#include "Vision/Renderer/Texture.h"

namespace Vision
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(glm::vec4* colors, uint32_t width, uint32_t height, const TextureProps& props);
		OpenGLTexture(const std::string& path, const TextureProps& props);
		
		~OpenGLTexture();

		void SetWrapOnX(WrapMode wrapMode) override;
		void SetWrapOnY(WrapMode wrapMode) override;
		void SetWrapBoth(WrapMode wrapModeX, WrapMode wrapModeY) override;
		void SetBorderColor(const glm::vec4& color) override;
		void SetFilterMode(FilterMode filterMode) override;

		inline const TextureProps& GetProps() const override { return m_Props; }

		void Bind(uint32_t slot) override;
		void UnBind() override;
		
		inline uint32_t GetWidth() override { return m_Width; }
		inline uint32_t GetHeight() override { return m_Height; }

	private:
		uint32_t GetWrapModeOpenGL(WrapMode wrapMode);
		void ApplyFilter(FilterMode filterMode);

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;
		TextureProps m_Props;
	};
}