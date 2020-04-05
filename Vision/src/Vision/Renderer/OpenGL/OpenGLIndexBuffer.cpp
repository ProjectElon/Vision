#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace Vision
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(const BufferProps& props)
		: m_DataType(props.DataType)
		, m_Size(props.Size)
	{
		uint32_t typeSize = GetDataTypeSize(props.DataType);
		m_Count = m_Size / typeSize;

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

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, props.Data, glUsage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_IBO);
	}

	void OpenGLIndexBuffer::SetData(void* data, uint32_t size, uint32_t offset) const
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
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