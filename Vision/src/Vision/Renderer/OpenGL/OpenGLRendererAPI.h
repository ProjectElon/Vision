#pragma once

#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		OpenGLRendererAPI();

		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;

		void SetClearColor(const glm::vec4& color) const override;
		void Clear(uint32_t flags) const override;

		void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t count, Primitive primitive) const override;
	};
}