#pragma once

#include "pch.h"
#include "Vision/Renderer/APIDefs.h"

namespace Vision
{
	struct VertexAttribute
	{
		API::DataType Type;
		const char* Name;
		bool Normalized = false;
	};

	class VertexLayout
	{
	public:
		VertexLayout(const std::vector<VertexAttribute>& attributes);
		~VertexLayout();
		
		inline const std::vector<VertexAttribute>& GetAttributes() const { return m_Attributes; }
		
		uint32_t CalculateVertexSize() const;

	private:
		std::vector<VertexAttribute> m_Attributes;
	};
}