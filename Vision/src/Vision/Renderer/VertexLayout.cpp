#include "pch.h"
#include "VertexLayout.h"

namespace Vision
{
	VertexLayout::VertexLayout(const std::vector<VertexAttribute>& attributes)
		: m_Attributes(attributes)
	{
	}

	uint32_t VertexLayout::CalculateVertexSize() const
	{
		uint32_t vertexSize = 0;

		for (const auto& attribute : m_Attributes)
		{
			vertexSize += Renderer::GetDataTypeSize(attribute.Type);
		}

		return vertexSize;
	}

	VertexLayout::~VertexLayout()
	{
	}
}