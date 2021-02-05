#pragma once

#include "pch.h"
#include "Vision/Renderer/Renderer.h"

#include <initializer_list>

namespace Vision
{
	struct VertexAttribute
	{
		ShaderDataType	Type;
		const char* 	Name;
		bool        	Normalized;
	};

	struct VertexLayout
	{
		VertexLayout(const std::initializer_list<VertexAttribute>& attributes)
			: Attributes(attributes)
		{
		}

		std::vector<VertexAttribute> Attributes;
	};
}