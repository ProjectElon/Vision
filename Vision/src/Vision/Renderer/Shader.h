#pragma once

#include "Vision/Core/Core.h"

#include <cstdint>
#include <glm/glm.hpp>

namespace Vision
{
	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual const std::string& GetName() const = 0;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void Reload() = 0;

		virtual void SetInt(const std::string& name, int32_t value) = 0;
		virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& vec2) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& vec3) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& vec4) = 0;

		virtual void SetMatrix3(const std::string& name, const glm::mat3& mat3) = 0;
		virtual void SetMatrix4(const std::string& name, const glm::mat4& mat4) = 0;

		static Ref<Shader> CreateFromFile(const std::string& filepath);
	};
}