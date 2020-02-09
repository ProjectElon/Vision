#pragma once

#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		void OnWindowResize(uint32_t width, uint32_t height) const override;

		void SetClearColor(const glm::vec4& color) const override;
		void Clear(uint32_t flags) const override;

		void DrawIndexed(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer) const override;
	};
}