#pragma once

#include "Vision/Renderer/RendererAPI.h"
#include <glad/glad.h>

namespace Vision
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		OpenGLRendererAPI();

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;

		void SetClearColor(const glm::vec4& color) const override;
		void Clear(uint32_t flags) const override;

		void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t count) const override;

		int32_t GetMaxTextureSlots() const override;

		inline static uint32_t MapDataTypeToGLType(API::DataType dataType)
		{
			return s_TypeMap[(int)dataType];
		}

	private:
		static uint32_t s_TypeMap[];
	};
}