#pragma once

#include "Vision/Renderer/Shader.h"

namespace Vision
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		~OpenGLShader();

		void Bind() const override;
		void UnBind() const override;

		inline const std::string& GetName() const { return m_Name; }

	private:
		uint32_t m_VertexShader;
		uint32_t m_PixelShader;
		uint32_t m_Program;

		std::string m_Name;
	};
}