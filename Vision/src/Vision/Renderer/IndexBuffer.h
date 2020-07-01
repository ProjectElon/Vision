#pragma once

#include "Vision/Renderer/Buffer.h"
#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
	class IndexBuffer : public Buffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual RendererAPI::DataType GetDataType() const = 0;
		virtual uint32_t GetIndexCount() const = 0;

		static Ref<IndexBuffer> Create(const BufferProps& props);
	};
}