#pragma once

#include "Vision/Core/Defines.h"

namespace Vision
{
	enum class BufferUsage : uint32
	{
		Static,
		Dynamic
	};

	struct VertexBuffer
	{
		union
		{
#ifdef VN_RENDERER_API_OPENGL
			struct
			{
				uint32 VertexBufferObject;
				uint32 VertexArrayObject;
			}
			OpenGL;
#endif
		};

		memorysize 	SizeInBytes;
		BufferUsage Usage;
	};

	struct IndexBuffer
	{
		union
		{
#ifdef VN_RENDERER_API_OPENGL
			struct
			{
				uint32 ElementBufferObject;
			}
			OpenGL;
#endif
		};

		memorysize  SizeInBytes;
		BufferUsage Usage;
	};

#ifdef VN_RENDERER_API_OPENGL

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

#endif
}