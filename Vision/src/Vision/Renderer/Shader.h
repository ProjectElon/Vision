#pragma once

#include <cstdint>

namespace Vision
{
	class Shader
	{
	public:
		enum class DataType : uint8_t
		{
			Bool,
			Int,
			Int2,
			Int3,
			Int4,
			UInt,
			UInt2,
			UInt3,
			UInt4,
			Float,
			Float2,
			Float3,
			Float4,
			Matrix3,
			Matrix4
		};

		static uint32_t GetDataTypeSize(DataType dataType);
		static uint32_t GetComponentCount(DataType dataType);

	public:
		Shader() = default;
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const std::string& GetName() const = 0;

		static Shader* Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	};
}