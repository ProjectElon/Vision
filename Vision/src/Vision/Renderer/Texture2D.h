#pragma once

#include "Vision/Core/Core.h"

#include <glm/glm.hpp>

namespace Vision
{
	enum class WrapMode
	{
		Repeat = 0,
		ClampToEdge = 1,
	};

	enum class FilterMode
	{
		Point = 0,
		Bilinear = 1
	};

	struct TextureProps
	{
		WrapMode WrapX;
		WrapMode WrapY;
		FilterMode Filter;
		
		TextureProps()
			: WrapX(WrapMode::Repeat)
			, WrapY(WrapMode::Repeat)
			, Filter(FilterMode::Point)
		{
		}
	};

	class Texture2D
	{
	public:
		virtual ~Texture2D() {}
		
		virtual void SetData(void* data, uint32_t sizeInBytes) = 0;

		virtual void Bind(uint32_t slot) = 0;
		
		virtual void SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY) = 0;
		virtual void SetFilterMode(FilterMode filterMode) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		static Ref<Texture2D> Create(uint32_t width, uint32_t height, const TextureProps& props = TextureProps());
		static Ref<Texture2D> CreateFromFile(const std::string& path, const TextureProps& props = TextureProps());
	};
}