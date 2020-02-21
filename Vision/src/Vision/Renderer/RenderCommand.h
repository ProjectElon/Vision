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

		inline static void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, Primitive primitive = Primitive::Triangles)
		{
			Renderer::GetRendererAPI().DrawIndexed(vertexBuffer, indexBuffer, primitive);
		}
	};
}