#include "pch.h"
#include "VertexLayout.h"

namespace Vision
{
	VertexLayout::VertexLayout(const std::vector<VertexAttribute>& attributes)
		: m_Attributes(attributes)
		, m_Stride(0)
	{
		for (const auto& attribute : m_Attributes)
		{		
			m_Stride += Shader::GetDataTypeSize(attribute.Type);
		}
	}

	VertexLayout::~VertexLayout()
	{
	}
}