#pragma once

#include "Vision/Core/Core.h"

#include <cstdint>
#include <glm/glm.hpp>

namespace Vision
{
	class Shader
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

		virtual void SetInt(const std::string& name, int32_t value) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& vec2) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& vec3) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& vec4) = 0;

		virtual void SetMatrix3(const std::string& name, const glm::mat3& mat3) = 0;
		virtual void SetMatrix4(const std::string& name, const glm::mat4& mat4) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> CreateFromFile(const std::string& path);
	};
}