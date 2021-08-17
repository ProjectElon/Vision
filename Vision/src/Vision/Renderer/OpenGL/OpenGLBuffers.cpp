#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"

#ifdef VN_RENDERER_API_OPENGL

#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"
#include "Vision/Renderer/VertexLayout.h"
#include <glad/glad.h>

namespace Vision
{
	void OpenGLInitVertexBuffer(VertexBuffer* vertexBuffer,
	                            const void* data,
								uint32 sizeInBytes,
								BufferUsage usage)
	{
		vertexBuffer->SizeInBytes = sizeInBytes;
		vertexBuffer->Usage		  = usage;

		auto& openglVertexBuffer = vertexBuffer->OpenGL;

		glCreateVertexArrays(1, &openglVertexBuffer.VertexArrayObject);
		glBindVertexArray(openglVertexBuffer.VertexArrayObject);

		glCreateBuffers(1, &openglVertexBuffer.VertexBufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, openglVertexBuffer.VertexBufferObject);

		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, GLBufferUsage(usage));
		glBindVertexArray(0);
	}

	void OpenGLUninitVertexBuffer(VertexBuffer* vertexBuffer)
	{
		auto& openglVertexBuffer = vertexBuffer->OpenGL;
		glDeleteBuffers(1, &openglVertexBuffer.VertexBufferObject);
		glDeleteVertexArrays(1, &openglVertexBuffer.VertexArrayObject);
	}

	void OpenGLBindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		auto& openglVertexBuffer = vertexBuffer->OpenGL;
		glBindVertexArray(openglVertexBuffer.VertexArrayObject);
	}

	void OpenGLSetVertexBufferData(VertexBuffer* vertexBuffer,
	                           	   const void* data,
	                               uint32 sizeInBytes,
	                               uint32 offset)
	{
		VnCoreAssert(vertexBuffer->Usage == BufferUsage::Dynamic,
		             "can't set data to a static buffer");

		auto& openglVertexBuffer = vertexBuffer->OpenGL;
		glBindBuffer(GL_ARRAY_BUFFER, openglVertexBuffer.VertexBufferObject);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeInBytes, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLSetVertexBufferLayout(VertexBuffer* vertexBuffer,
	                                 VertexLayout* layout)
	{
		// @Incomplete: Note that this code will fail
		// if the alignment of the vertex layout is not 4 bytes
		// to solve this we need meta programming

		const auto& attributes = layout->Attributes;

		uint32 offset = 0;
		uint32 stride = 0; // vertex size in bytes

		for (uint32 vertexIndex = 0; vertexIndex < attributes.size(); ++vertexIndex)
		{
			stride += GLTypeSize(attributes[vertexIndex].Type);
		}

		glBindVertexArray(vertexBuffer->OpenGL.VertexArrayObject);

		for (uint32 vertexIndex = 0; vertexIndex < attributes.size(); ++vertexIndex)
		{
			const VertexAttribute& attribute = attributes[vertexIndex];
			glEnableVertexAttribArray(vertexIndex);

			switch (attribute.Type)
			{
				case ShaderDataType_Bool:
				case ShaderDataType_Int8:
				case ShaderDataType_UInt8:
				case ShaderDataType_Int16:
				case ShaderDataType_UInt16:
				case ShaderDataType_Int32:
				case ShaderDataType_UInt32:
				{
					glVertexAttribIPointer(vertexIndex,
										   GLTypeComponentCount(attribute.Type),
										   ShaderDataTypeToOpenGLType(attribute.Type),
										   stride,
										   (const void*)offset);
				}
				break;

				case ShaderDataType_Float:
				case ShaderDataType_Float2:
				case ShaderDataType_Float3:
				case ShaderDataType_Float4:
				{
					glVertexAttribPointer(vertexIndex,
										  GLTypeComponentCount(attribute.Type),
										  ShaderDataTypeToOpenGLType(attribute.Type),
										  attribute.Normalized ? GL_TRUE : GL_FALSE,
										  stride,
										  (const void*)offset);
				}
				break;


				case ShaderDataType_Matrix3:
				case ShaderDataType_Matrix4:
				{
					VnCoreAssert(false, "unsupported types");
				}
				break;
			}

			offset += GLTypeSize(attribute.Type);
		}

		glBindVertexArray(0);
	}

	static void InitIndexBuffer(IndexBuffer* indexBuffer,
	                            const void* data,
	                            memorysize sizeInBytes,
	                            BufferUsage usage)
	{
		indexBuffer->SizeInBytes = sizeInBytes;
		indexBuffer->Usage		 = usage;

		auto& openglIndexBuffer = indexBuffer->OpenGL;

		glCreateBuffers(1, &openglIndexBuffer.ElementBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglIndexBuffer.ElementBufferObject);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, data, GLBufferUsage(usage));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLInitIndexBuffer16(IndexBuffer* indexBuffer,
	                             const uint16* data,
	                             uint32 sizeInBytes,
	                             BufferUsage usage)
	{
		InitIndexBuffer(indexBuffer, data, sizeInBytes, usage);
	}

	void OpenGLInitIndexBuffer32(IndexBuffer* indexBuffer,
		                         const uint32* data,
					  			 uint32 sizeInBytes,
					  			 BufferUsage usage)
	{
		InitIndexBuffer(indexBuffer, data, sizeInBytes, usage);
	}

	void OpenGLUninitIndexBuffer(IndexBuffer* indexBuffer)
	{
		auto& openglIndexBuffer = indexBuffer->OpenGL;
		glDeleteBuffers(1, &openglIndexBuffer.ElementBufferObject);
	}

	void OpenGLBindIndexBuffer(IndexBuffer* indexBuffer)
	{
		auto& openglIndexBuffer = indexBuffer->OpenGL;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglIndexBuffer.ElementBufferObject);
	}
}

#endif