#pragma once

#include "Vision/Core/Core.h"

#include <glm/glm.hpp>

namespace Vision
{
	enum class WrapMode
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder
	};

	enum class FilterMode
	{
		Point,
		Bilinear
	};

	struct TextureProps
	{
		WrapMode WrapX;
		WrapMode WrapY;
		FilterMode Filter;
		glm::vec4 BorderColor;

		TextureProps()
			: WrapX(WrapMode::Repeat)
			, WrapY(WrapMode::Repeat)
			, Filter(FilterMode::Bilinear)
			, BorderColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
		{
		}
	};

	class Texture
	{
	public:
		virtual ~Texture() {}

		virtual void SetWrapOnX(WrapMode wrapMode) = 0;
		virtual void SetWrapOnY(WrapMode wrapMode) = 0;
		virtual void SetWrapBoth(WrapMode wrapModeX, WrapMode wrapModeY) = 0;
		virtual void SetBorderColor(const glm::vec4& color) = 0;
		virtual void SetFilterMode(FilterMode filterMode) = 0;

		virtual const TextureProps& GetProps() const = 0;

		virtual void SetData(void* data, uint32_t sizeInBytes) = 0;

		virtual void Bind(uint32_t slot) = 0;
		virtual void UnBind() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual const std::string& GetName() const = 0;

		static Ref<Texture> Create(uint32_t width, uint32_t height, const TextureProps& props = TextureProps());
		static Ref<Texture> CreateFromFile(const std::string& path, const TextureProps& props = TextureProps());
	};
}