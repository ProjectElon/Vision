#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLVertexBuffer.h"
#include "Vision/Renderer/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Vision
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(const BufferProps& props)
		: m_DataType(props.DataType)
		, m_Size(props.Size)
	{
		uint32_t typeSize = RendererAPI::GetDataTypeSize(props.DataType);
		m_Count = m_Size / typeSize;

		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glCreateBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		uint32_t glUsage = GL_NONE;

		switch (props.Usage)
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

		glBufferData(GL_ARRAY_BUFFER, m_Size, props.Data, glUsage);
		glBindVertexArray(0);
	}

	void OpenGLVertexBuffer::SetData(void* data, uint32_t size, uint32_t offset) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetLayout(const VertexLayout& layout) const
	{
		glBindVertexArray(m_VAO);

		const std::vector<VertexAttribute>& attributes = layout.GetAttributes();
		uint32_t offset = 0;
		uint32_t stride = layout.CalculateVertexSize();

		for (uint32_t i = 0; i < attributes.size(); ++i)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				RendererAPI::GetDataTypeComponentCount(attributes[i].Type),
				OpenGLRendererAPI::MapDataTypeToGLType(attributes[i].Type),
				attributes[i].Normalized,
				stride,
				(const void*)offset
			);

			offset += RendererAPI::GetDataTypeSize(attributes[i].Type);
		}

		glBindVertexArray(0);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_VBO);
		glDeleteVertexArrays(1, &m_VAO);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindVertexArray(m_VAO);
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		glBindVertexArray(0);
	}
}