#pragma once

#include "Vision/Core/Core.h"
#include "Vision/Renderer/VertexBuffer.h"
#include "Vision/Renderer/IndexBuffer.h"
#include "Vision/Renderer/APIDefs.h"

#include <glm/glm.hpp>

namespace Vision
{
	class RendererAPI
	{
	public:
		virtual void Clear(uint32_t clearflags) const = 0;
		virtual void SetClearColor(const glm::vec4& color) const = 0;
		
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, uint32_t count) const = 0;

		virtual int32_t GetMaxTextureSlots() const = 0;
	};
}