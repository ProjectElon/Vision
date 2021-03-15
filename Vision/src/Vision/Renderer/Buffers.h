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
		uint32 RendererLayoutID = 0;
		uint32 RendererID = 0;

		uint32 SizeInBytes = 0;
		BufferUsage Usage = BufferUsage::Static;

		VertexLayout Layout;

		VertexBuffer() = default;
		VertexBuffer(const void* data,
				 	 uint32 sizeInBytes,
				  	 BufferUsage usage);

		~VertexBuffer();

		void Init(const void* data,
				  uint32 sizeInBytes,
				  BufferUsage usage);

		void Uninit();

		void SetLayout(const VertexLayout& vertexLayout);

		void SetData(const void* data,
					 uint32 sizeInBytes,
					 uint32 offset = 0);

		void Bind();
		void Unbind();
	};

	struct IndexBuffer
	{
		uint32 RendererID = 0;
		uint32 SizeInBytes = 0;

		BufferUsage Usage = BufferUsage::Static;

		IndexBuffer() = default;

		IndexBuffer(const uint32* data,
					uint32 sizeInBytes,
					BufferUsage usage = BufferUsage::Static);

		~IndexBuffer();

		void Init(const uint32* data,
				  uint32 sizeInBytes,
				  BufferUsage usage = BufferUsage::Static);

		void Uninit();

		void Bind();
		void Unbind();
	};
}