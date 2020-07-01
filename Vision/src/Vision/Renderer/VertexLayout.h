#pragma once

#include "pch.h"
#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
	struct VertexAttribute
	{
		RendererAPI::DataType Type;
		const char*           Name;
		bool                  Normalized;
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