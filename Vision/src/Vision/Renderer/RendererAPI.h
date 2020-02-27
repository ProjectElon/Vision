#pragma once

#include <glm/glm.hpp>

#include "Vision/Core/Core.h"
#include "Vision/Renderer/Buffers.h"

namespace Vision
{
	enum ClearFlags : uint32_t
	{
		ClearColorBuffer   = VN_BIT(1),
		ClearDepthBuffer   = VN_BIT(2),
		ClearStencilBuffer = VN_BIT(3)
	};

	enum class Primitive : uint8_t
	{
		Points,
		Lines,
		LineStrip,
		Quads,
		Triangles,
		TriangleStrip,
		TriangleFan
	};

	class RendererAPI
	{
	public:
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;
		
		virtual void SetClearColor(const glm::vec4& color) const = 0;

		virtual void Clear(uint32_t clearflags) const = 0;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t count, Primitive primitive = Primitive::Triangles) const = 0;
	};
}