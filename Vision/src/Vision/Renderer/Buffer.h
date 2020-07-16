#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
	enum class BufferUsage : uint8_t
	{
		Static,
		Dynamic
	};

	struct BufferProps
	{
		void*                 Data;
		RendererAPI::DataType DataType;
		uint32_t              Size;
		BufferUsage           Usage;
	};

	class Buffer
	{
	public:
		virtual ~Buffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(void* data, uint32_t size, uint32_t offset = 0) const = 0;

		virtual uint32_t GetSize() const = 0;
	};
}