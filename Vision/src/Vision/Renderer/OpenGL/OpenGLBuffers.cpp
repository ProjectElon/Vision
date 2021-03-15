#include "pch.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

#include <glad/glad.h>

namespace Vision
{

	VertexBuffer::VertexBuffer(const void* data,
				 	 		   uint32 sizeInBytes,
				  	 		   BufferUsage usage)
	{
		Init(data, sizeInBytes, usage);
	}

	VertexBuffer::~VertexBuffer()
	{
		Uninit();
	}

	void VertexBuffer::Init(const void* data,
							uint32 sizeInBytes,
							BufferUsage usage)
	{
		SizeInBytes = sizeInBytes;
		Usage = usage;

		glCreateVertexArrays(1, &RendererLayoutID);
		glBindVertexArray(RendererLayoutID);

		glCreateBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);

		glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, GLBufferUsage(usage));
		glBindVertexArray(0);
	}

	void VertexBuffer::Uninit()
	{
		glDeleteBuffers(1, &RendererID);
		glDeleteVertexArrays(1, &RendererLayoutID);
	}

	void VertexBuffer::Bind()
	{
		glBindVertexArray(RendererLayoutID);
	}

	void VertexBuffer::Unbind()
	{
		glBindVertexArray(0);
	}

	void VertexBuffer::SetData(const void* data,
	                           uint32 sizeInBytes,
	                           uint32 offset)
	{
		VnCoreAssert(Usage == BufferUsage::Dynamic, "can't set data to a static buffer");
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeInBytes, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::SetLayout(const VertexLayout& layout)
	{
		// @(Harlequin): Note that this code will fail
		// if the alignment of the vertex layout is not 4 bytes
		// to solve this we need meta programming

		Layout = layout;

		const auto& attributes = layout.Attributes;

		uint32 offset = 0;
		uint32 stride = 0; // vertex size in bytes

		for (uint32 vertexIndex = 0; vertexIndex < attributes.size(); ++vertexIndex)
		{
			stride += GLTypeSize(attributes[vertexIndex].Type);
		}

		glBindVertexArray(RendererLayoutID);

		for (uint32 vertexIndex = 0; vertexIndex < attributes.size(); ++vertexIndex)
		{
			const VertexAttribute& attribute = attributes[vertexIndex];
			glEnableVertexAttribArray(vertexIndex);

			switch (attribute.Type)
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



	IndexBuffer::IndexBuffer(const uint32* data,
							 uint32 sizeInBytes,
							 BufferUsage usage)
	{
		Init(data, sizeInBytes, usage);
	}

	IndexBuffer::~IndexBuffer()
	{
		Uninit();
	}

	void IndexBuffer::Init(const uint32* data,
						   uint32 sizeInBytes,
						   BufferUsage usage)
	{
		SizeInBytes = sizeInBytes;
		Usage = usage;

		glCreateBuffers(1, &RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeInBytes, data, GLBufferUsage(usage));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IndexBuffer::Uninit()
	{
		glDeleteBuffers(1, &RendererID);
	}

	void IndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID);
	}

	void IndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}