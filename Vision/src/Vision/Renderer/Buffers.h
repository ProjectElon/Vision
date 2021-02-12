#pragma once

#include "Vision/Core/Common.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/VertexLayout.h"

namespace Vision
{
	enum class BufferUsage
	{
		Static,
		Dynamic
	};

	struct VertexBuffer
	{
		uint32 BufferLayoutID = 0;
		uint32 BufferID = 0;

		uint32 SizeInBytes = 0;		
		BufferUsage Usage = BufferUsage::Static;
	};

	void CreateVertexBuffer(VertexBuffer* vertexBuffer,
							const float32* data,
							uint32 sizeInBytes,
							BufferUsage usage);

	void DestroyVertexBuffer(VertexBuffer* buffer);

	void BindVertexBuffer(VertexBuffer* vertexBuffer);
	void UnBindVertexBuffer(VertexBuffer* vertexBuffer);

	void SetVertexBufferLayout(VertexBuffer* vertexBuffer,
							   const VertexLayout* vertexLayout);

	void SetVertexBufferData(VertexBuffer* vertexBuffer,
							 const float* data,
							 uint32 sizeInBytes,
							 uint32 offset = 0);

	struct IndexBuffer
	{
		uint32 BufferID	= 0;
	};

	void CreateIndexBuffer(IndexBuffer* indexBuffer,
						   const uint32* data,
						   uint32 sizeInBytes);

	void DestroyIndexBuffer(IndexBuffer* indexBuffer);

	void BindIndexBuffer(IndexBuffer* vertexBuffer);
	void UnBindIndexBuffer(IndexBuffer* vertexBuffer);
}