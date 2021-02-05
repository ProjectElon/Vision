#include "pch.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

#include <glad/glad.h>

namespace Vision
{
	void CreateVertexBuffer(VertexBuffer* vertexBuffer,
							const float32* data,
							uint32 sizeInBytes,
							BufferUsage usage)
	{
		vertexBuffer->SizeInBytes = sizeInBytes;
		vertexBuffer->Usage = usage;

		glCreateVertexArrays(1, &vertexBuffer->BufferLayoutID);
		glBindVertexArray(vertexBuffer->BufferLayoutID);

		glCreateBuffers(1, &vertexBuffer->BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->BufferID);

		uint32_t glUsage = GL_NONE;

		switch (usage)
		{
			case BufferUsage::Static:
			{
				glUsage = GL_STATIC_DRAW;
			}
			break;

			case BufferUsage::Dynamic:
			{
				glUsage = GL_DYNAMIC_DRAW;
			}
			break;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, glUsage);
		glBindVertexArray(0);
	}

	void DestroyVertexBuffer(VertexBuffer* buffer)
	{
		glDeleteBuffers(1, &buffer->BufferID);
		glDeleteVertexArrays(1, &buffer->BufferLayoutID);
	}

	void BindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		glBindVertexArray(vertexBuffer->BufferLayoutID);
	}

	void UnBindVertexBuffer(VertexBuffer* vertexBuffer)
	{
		glBindVertexArray(0);
	}

	void SetVertexBufferData(VertexBuffer* vertexBuffer, const float* data, uint32 sizeInBytes, uint32 offset)
	{
		VnCoreAssert(vertexBuffer->Usage == BufferUsage::Dynamic, "can't set data to a static buffer");
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeInBytes, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SetVertexBufferLayout(VertexBuffer* vertexBuffer,
							   const VertexLayout* vertexLayout)
	{
		// @(Harlequin): Note that this code will fail
		// if the alignment of the vertex layout is not 4 bytes
		// to solve this we need meta programming

		const auto& attributes = vertexLayout->Attributes;

		uint32 offset = 0;
		uint32 stride = 0; // vertex size in bytes

		for (uint32 vertexIndex = 0; vertexIndex < attributes.size(); ++vertexIndex)
		{
			stride += GLTypeSize(attributes[vertexIndex].Type);
		}

		glBindVertexArray(vertexBuffer->BufferLayoutID);

		for (uint32 vertexIndex = 0; vertexIndex < attributes.size(); ++vertexIndex)
		{
			const VertexAttribute& attribute = attributes[vertexIndex];
			glEnableVertexAttribArray(vertexIndex);

			switch(attribute.Type)
			{
				case ShaderDataType::Bool:
				case ShaderDataType::Int8:
				case ShaderDataType::UInt8:
				case ShaderDataType::Int16:
				case ShaderDataType::UInt16:
				case ShaderDataType::Int32:
				case ShaderDataType::UInt32:
				{
					glVertexAttribIPointer(vertexIndex,
										   GLTypeComponentCount(attribute.Type),
										   ShaderDataTypeToOpenGLType(attribute.Type),
										   stride,
										   (const void*)offset);
				}
				break;

				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				case ShaderDataType::Matrix3:
				case ShaderDataType::Matrix4:
				{
					glVertexAttribPointer(vertexIndex,
										  GLTypeComponentCount(attribute.Type),
										  ShaderDataTypeToOpenGLType(attribute.Type),
										  attribute.Normalized ? GL_TRUE : GL_FALSE,
										  stride,
										  (const void*)offset);
				}
				break;
			}

			offset += GLTypeSize(attribute.Type);
		}

		glBindVertexArray(0);
	}

	void CreateIndexBuffer(IndexBuffer* indexBuffer,
						   const uint32* data,
						   uint32 sizeInBytes)
	{
		glCreateBuffers(1, &indexBuffer->BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->BufferID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void DestroyIndexBuffer(IndexBuffer* indexBuffer)
	{
		glDeleteBuffers(1, &indexBuffer->BufferID);
	}

	void BindIndexBuffer(IndexBuffer* indexBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->BufferID);
	}

	void UnBindIndexBuffer(IndexBuffer* vertexBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}