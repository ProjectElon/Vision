#pragma once

#include "pch.h"
#include "Vision/Renderer/Shader.h"

namespace Vision
{
	struct VertexAttribute
	{
		Shader::DataType Type;
		const char* Name;
		bool Normalized;
	};

	class VertexLayout
	{
	public:
		VertexLayout(const std::vector<VertexAttribute>& attributes);
		~VertexLayout();
		
		inline const std::vector<VertexAttribute>& GetAttributes() const { return m_Attributes; }
		
		inline uint32_t GetStride() const { return m_Stride; }

	private:
		std::vector<VertexAttribute> m_Attributes;
		uint32_t m_Stride;
	};
}