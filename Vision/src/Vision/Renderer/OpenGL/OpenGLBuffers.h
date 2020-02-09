#pragma once

#include "Vision/Renderer/Buffers.h"

namespace Vision
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const BufferProps& props);
		~OpenGLVertexBuffer();

		void Bind() const override;
		void UnBind() const override;

		void SetLayout(const VertexLayout& layout) const override;

		inline uint32_t GetSizeInBytes() const override { return m_SizeInBytes; }
		inline uint32_t GetVertexCount() const override { return m_Count; }

	private:
		uint32_t m_VBO, m_VAO;
		uint32_t m_Count;
		uint32_t m_SizeInBytes;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const BufferProps& props);
		~OpenGLIndexBuffer();

		void Bind() const override;
		void UnBind() const override;

		inline uint32_t GetSizeInBytes() const override { return m_SizeInBytes; }
		inline uint32_t GetIndexCount() const override { return m_Count; }

	private:
		uint32_t m_IBO;
		uint32_t m_Count;
		uint32_t m_SizeInBytes;
	};
}