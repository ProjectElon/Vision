#pragma once

#include "Vision/Renderer/VertexLayout.h"

namespace Vision
{
	enum class BufferUsage
	{
		Static,  /* Read only by the GPU */
		Dynamic, /* Read only by the GPU and Write only CPU */
		Default  /* Read and 'write' by the GPU */
	};

	struct BufferProps
	{
		void* Data;
		Shader::DataType DataType;
		uint32_t SizeInBytes;
		BufferUsage Usage;
	};

	class Buffer
	{
	public:
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		
		virtual uint32_t GetSizeInBytes() const = 0;
	};

	class VertexBuffer : public Buffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void SetLayout(const VertexLayout& layout) const = 0;
		
		virtual uint32_t GetVertexCount() const = 0;
		
		static VertexBuffer* Create(const BufferProps& props);
	};

	class IndexBuffer : public Buffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual uint32_t GetIndexCount() const = 0;

		static IndexBuffer* Create(const BufferProps& props);
	};
}