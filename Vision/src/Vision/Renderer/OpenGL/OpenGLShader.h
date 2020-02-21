#pragma once

#include "Vision/Renderer/Shader.h"

#include <map>

namespace Vision
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& path);
		~OpenGLShader();

		void Bind() const override;
		void UnBind() const override;

		inline const std::string& GetName() const { return m_Name; }

		void UploadUniformInt(const std::string& name, int32_t value) override;

		void UploadUniformFloat(const std::string& name, float value) override;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vec2) override;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vec3) override;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vec4) override;

		void UploadUniformMatrix3(const std::string& name, const glm::mat3& mat3) override;
		void UploadUniformMatrix4(const std::string& name, const glm::mat4& mat4) override;

	private:
		void CompileSubShader(uint32_t subShader, const std::string& source);
		void LinkSubShaders();

		int32_t GetUniformLocation(const std::string& name);

	private:
		std::string m_Name;

		uint32_t m_VertexShader;
		uint32_t m_PixelShader;
		uint32_t m_GeoShader;
		
		uint32_t m_RendererID;

		std::unordered_map<std::string, int32_t> m_UniformLocations;
	};
}