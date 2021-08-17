#pragma once

#include "pch.hpp"
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
		std::vector<VertexAttribute> Attributes;

		VertexLayout() = default;

		VertexLayout(const std::initializer_list<VertexAttribute>& attributes)
			: Attributes(attributes)
		{
		}
	};
}