#include "pch.h"
#include "Vision/Renderer/Buffers.h"

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

	static GLenum GetGLType(DataType dataType)
	{
		switch (dataType)
        {
            case DataType::Bool:    return GL_BOOL;           break;
            case DataType::Int8:    return GL_BYTE;			  break;
            case DataType::UInt8:   return GL_UNSIGNED_BYTE;  break;
            case DataType::Int16:   return GL_SHORT;		  break;
            case DataType::UInt16:  return GL_UNSIGNED_SHORT; break;
            case DataType::Int32:   return GL_INT;            break;
            case DataType::UInt32:  return GL_UNSIGNED_INT;   break;
            case DataType::Float:   return GL_FLOAT;		  break;
            case DataType::Float2:  return GL_FLOAT;		  break;
            case DataType::Float3:  return GL_FLOAT;		  break;
            case DataType::Float4:  return GL_FLOAT;		  break;
            case DataType::Matrix3: return GL_FLOAT;		  break;
            case DataType::Matrix4: return GL_FLOAT;		  break;
        }
	}

	void SetVertexBufferLayout(VertexBuffer* vertexBuffer,
							   const VertexLayout* vertexLayout)
	{
		glBindVertexArray(vertexBuffer->BufferLayoutID);

		const std::vector<VertexAttribute>& attributes = vertexLayout->GetAttributes();
		uint32_t offset = 0;
		uint32_t stride = vertexLayout->CalculateVertexSize();

		for (uint32_t i = 0; i < attributes.size(); ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				Renderer::GetDataTypeComponentCount(attributes[i].Type),
				GetGLType(attributes[i].Type),
				attributes[i].Normalized,
				stride,
				(const void*)offset
			);

			offset += Renderer::GetDataTypeSize(attributes[i].Type);
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