#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLBuffers.h"
#include "Vision/Renderer/Shader.h"
#include <glad/glad.h>

namespace Vision
{
	/*---------- Vertex Buffer ----------*/

	OpenGLVertexBuffer::OpenGLVertexBuffer(const BufferProps& props)
	{
		uint32_t typeSize = Shader::GetDataTypeSize(props.DataType);
		m_SizeInBytes = props.SizeInBytes;
		m_Count = props.SizeInBytes / typeSize;

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

		glBufferData(GL_ARRAY_BUFFER, props.SizeInBytes, props.Data, glUsage);
		glBindVertexArray(0);
	}

	void OpenGLVertexBuffer::SetLayout(const VertexLayout& layout) const
	{
		glBindVertexArray(m_VAO);

		const std::vector<VertexAttribute>& attributes = layout.GetAttributes();
		uint32_t offset = 0;

		for (uint32_t i = 0; i < attributes.size(); ++i)
		{
			uint32_t componentCount = Shader::GetComponentCount(attributes[i].Type);
			
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				componentCount,
				GL_FLOAT,
				attributes[i].Normalized,
				layout.GetStride(),
				(const void*)offset
			);

			offset += Shader::GetDataTypeSize(attributes[i].Type);
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

	/*---------- Index Buffer ----------*/

	OpenGLIndexBuffer::OpenGLIndexBuffer(const BufferProps& props)
	{
		uint32_t typeSize = Shader::GetDataTypeSize(props.DataType);
		m_SizeInBytes = props.SizeInBytes;
		m_Count = props.SizeInBytes / typeSize;

		glCreateBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

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

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, props.SizeInBytes, props.Data, glUsage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_IBO);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}