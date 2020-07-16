#pragma once

#include "Vision/Core/Base.h"
#include <glm/glm.hpp>

namespace Vision
{
	class VertexBuffer;
	class IndexBuffer;

	class RendererAPI
	{
	public:
		enum class DataType : uint8_t
		{
			Bool,
			Byte,
			UByte,
			Short,
			UShort,
			Int,
			UInt,
			Float,
			Float2,
			Float3,
			Float4,
			Matrix3,
			Matrix4
		};

		enum ClearFlag : uint32_t
		{
			ColorBuffer   = VN_BIT(1),
			DepthBuffer   = VN_BIT(2),
			StencilBuffer = VN_BIT(3)
		};

		enum class Primitive : uint8_t
		{
			Lines,
			Triangles
		};
		
		static uint32_t GetDataTypeSize(DataType dataType);
		static uint32_t GetDataTypeComponentCount(DataType dataType);
		
	public:
		virtual void Clear(uint32_t clearflags) const = 0;
		virtual void SetClearColor(const glm::vec4& color) const = 0;
		
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;

		virtual void DrawIndexed(const Ref<VertexBuffer>& vertexBuffer,
								 const Ref<IndexBuffer>& indexBuffer,
								 uint32_t count,
								 Primitive primitive = Primitive::Triangles) const = 0;

		virtual int32_t GetMaxTextureSlots() const = 0;
	};
}