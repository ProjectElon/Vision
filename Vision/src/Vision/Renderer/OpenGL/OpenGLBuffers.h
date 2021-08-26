#pragma once

#include "Vision/Renderer/RendererTypes.h"

namespace Vision
{
	void OpenGLInitVertexBuffer(VertexBuffer* vertexBuffer,
		const void* data,
		uint32 sizeInBytes,
		BufferUsage usage);

	void OpenGLUninitVertexBuffer(VertexBuffer* vertexBuffer);

	void OpenGLSetVertexBufferLayout(VertexBuffer* vertexBuffer,
		struct VertexLayout* vertexLayout);

	void OpenGLSetVertexBufferData(VertexBuffer* vertexBuffer,
		const void* data,
		uint32 sizeInBytes,
		uint32 offset = 0);

	void OpenGLBindVertexBuffer(VertexBuffer* vertexBuffer);

	void OpenGLInitIndexBuffer16(IndexBuffer* indexBuffer,
		const uint16* data,
		uint32 sizeInBytes,
		BufferUsage usage);

	void OpenGLUninitIndexBuffer16(IndexBuffer* indexBuffer);
	
	void OpenGLInitIndexBuffer32(IndexBuffer* indexBuffer,
		const uint32* data,
		uint32 sizeInBytes,
		BufferUsage usage);

	void OpenGLUninitIndexBuffer32(IndexBuffer* indexBuffer);
	void OpenGLBindIndexBuffer(IndexBuffer* indexBuffer);
}