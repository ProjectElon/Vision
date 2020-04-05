#pragma once

#include "Vision/Renderer/VertexBuffer.h"

namespace Vision
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const BufferProps& props);
		~OpenGLVertexBuffer();

		void Bind() const override;
		void UnBind() const override;

		void SetData(void* data, uint32_t size, uint32_t offset = 0) const override;

		void SetLayout(const VertexLayout& layout) const override;

		inline uint32_t GetSize() const override { return m_Size; }
		inline uint32_t GetVertexCount() const override { return m_Count; }
		inline API::DataType GetDataType() const override { return m_DataType; }

	private:
		uint32_t m_VAO;
		uint32_t m_VBO;

		uint32_t m_Count;
		uint32_t m_Size;
		API::DataType m_DataType;
	};
}