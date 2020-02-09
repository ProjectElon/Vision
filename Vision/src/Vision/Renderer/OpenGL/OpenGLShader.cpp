#include "pch.h"
#include "Vision/Core/Log.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"

#include <glad/glad.h>

namespace Vision
{
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		: m_Name(name)
	{
		int success;
		char logInfo[512];

		/*---------- Vertex Shader ----------*/
		m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char* source = vertexSource.c_str();
		glShaderSource(m_VertexShader, 1, &source, nullptr);
		glCompileShader(m_VertexShader);

		glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(m_VertexShader, 512, nullptr, logInfo);
			VN_CORE_ERROR("SHADER {0} : Vertex Shader Compilation Failed ... \n {1}", name, logInfo);
		}

		/*---------- Fragment Shader ----------*/
		m_PixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		source = fragmentSource.c_str();
		glShaderSource(m_PixelShader, 1, &source, nullptr);
		glCompileShader(m_PixelShader);

		glGetShaderiv(m_PixelShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(m_PixelShader, 512, nullptr, logInfo);
			VN_CORE_ERROR("SHADER {0} : Pixel Shader Compilation Failed ... \n {1}", name, logInfo);
		}

		m_Program = glCreateProgram();
		glAttachShader(m_Program, m_VertexShader);
		glAttachShader(m_Program, m_PixelShader);
		glLinkProgram(m_Program);

		glGetProgramiv(m_Program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(m_Program, 512, nullptr, logInfo);
			VN_CORE_INFO("SHADER {0} : Program Linking Failed .. \n {1}", name, logInfo);
		}

		VN_CORE_TRACE("SHADER {0} : Completed Successfully", name);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDetachShader(m_Program, m_VertexShader);
		glDetachShader(m_Program, m_PixelShader);
		glDeleteShader(m_VertexShader);
		glDeleteShader(m_PixelShader);
		glDeleteProgram(m_Program);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_Program);
	}
	
	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}
}