#pragma once

#include "Vision/Renderer/IndexBuffer.h"

namespace Vision
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const BufferProps& props);
		~OpenGLIndexBuffer();

		void SetData(void* data, uint32_t sizeInBytes, uint32_t offset = 0) const override;

		void Bind() const override;
		void UnBind() const override;

		inline uint32_t GetSize() const override { return m_Size; }
		inline uint32_t GetIndexCount() const override { return m_Count; }
		inline API::DataType GetDataType() const override { return m_DataType; }

	private:
		uint32_t m_IBO;
		uint32_t m_Count;
		uint32_t m_Size;
		API::DataType m_DataType;
	};
}