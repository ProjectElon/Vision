#pragma once

#include "Vision/Renderer/Buffer.h"
#include "Vision/Renderer/VertexLayout.h"
#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
	class VertexBuffer : public Buffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void SetLayout(const VertexLayout& layout) const = 0;

		virtual RendererAPI::DataType GetDataType() const = 0;
		virtual uint32_t GetVertexCount() const = 0;

		static Ref<VertexBuffer> Create(const BufferProps& props);
	};
}