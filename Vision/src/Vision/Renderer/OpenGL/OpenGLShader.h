#pragma once

#include "Vision/Renderer/Shader.h"

#include <map>

#include <glad/glad.h>

namespace Vision
{
	struct ShaderData
	{
		std::string Source;
		int32_t RendererID;
	};

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& path);
		~OpenGLShader();

		void Bind() const override;
		void UnBind() const override;

		inline const std::string& GetName() const { return m_Name; }

		void SetInt(const std::string& name, int32_t value) override;

		void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const glm::vec2& vec2) override;
		void SetFloat3(const std::string& name, const glm::vec3& vec3) override;
		void SetFloat4(const std::string& name, const glm::vec4& vec4) override;

		void SetMatrix3(const std::string& name, const glm::mat3& mat3) override;
		void SetMatrix4(const std::string& name, const glm::mat4& mat4) override;

	public:
		static GLenum GetGLTypeFromShaderType(Shader::DataType dataType);

	private:
		void CompileShader(const ShaderData& shaderData);
		void LinkShaders();

		GLenum GetShaderTypeFromString(const std::string& type);
		int32_t GetUniformLocation(const std::string& name);
		
	private:
		uint32_t m_RendererID;

		std::string m_Name;
		std::unordered_map<GLenum, ShaderData> m_Shaders;
		std::unordered_map<std::string, int32_t> m_UniformLocations;
	};
}