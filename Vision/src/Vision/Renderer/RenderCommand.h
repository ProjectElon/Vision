#pragma once

#include "Vision/Renderer/Renderer.h"

namespace Vision
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color)
		{
			Renderer::GetRendererAPI().SetClearColor(color);
		}

		inline static void Clear(uint32_t flags)
		{
			Renderer::GetRendererAPI().Clear(flags);
		}

		inline static void OnWindowResize(uint32_t width, uint32_t height)
		{
			Renderer::GetRendererAPI().OnWindowResize(width, height);
		}

		inline static void DrawIndexed(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
		{
			Renderer::GetRendererAPI().DrawIndexed(vertexBuffer, indexBuffer);
		}
	};
}