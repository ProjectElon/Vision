#include "pch.h"
#include "Vision/Core/Log.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"
#include "Vision/Utils/StringUtils.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Vision
{
	OpenGLShader::OpenGLShader(const std::string& path)
		: m_RendererID(0)
		, m_VertexShader(0)
		, m_PixelShader(0)
		, m_GeoShader(0)
	{
		size_t lastSlash = path.find_last_of('/');
		m_Name = path.substr(lastSlash + 1);

		std::ifstream ifs(path);

		if (!ifs.is_open())
		{
			VN_CORE_ERROR("[SHADER]({0}) Unable to open file : {1}", m_Name, path);
			return;
		}
		
		uint32_t vertexIndex = 0;
		uint32_t pixelIndex = 1;
		uint32_t geoIndex = 2;

		uint32_t currentShaderIndex = -1;

		std::string source[3];
		std::string line;

		while (std::getline(ifs, line))
		{
			StringUtils::Trim(line);

			if (line.empty() || line.front() == '\n')
			{
				continue;
			}
			else if (line == "#vertex")
			{
				currentShaderIndex = vertexIndex;
			}
			else if (line == "#pixel")
			{
				currentShaderIndex = pixelIndex;
			}
			else if (line == "#geo")
			{
				currentShaderIndex = geoIndex;
			}
			else
			{
				source[currentShaderIndex] += line + '\n';
			}
		}

		if (source[vertexIndex].empty())
		{
			VN_CORE_ERROR("[SHADER]({0}) : Missing Vertex SubShader Source", m_Name);
		}
		else
		{
			m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
			CompileSubShader(m_VertexShader, source[vertexIndex]);
		}
		
		if (source[pixelIndex].empty())
		{
			VN_CORE_ERROR("[SHADER]({0}) : Missing Pixel SubShader Source", m_Name);
		}
		else
		{
			m_PixelShader = glCreateShader(GL_FRAGMENT_SHADER);
			CompileSubShader(m_PixelShader, source[pixelIndex]);
		}

		if (source[geoIndex].empty())
		{
			VN_CORE_WARN("[SHADER]({0}) : Missing Geometry SubShader Source", m_Name);
		}
		else
		{
			m_GeoShader = glCreateShader(GL_GEOMETRY_SHADER);
			CompileSubShader(m_GeoShader, source[geoIndex]);
		}
		
		LinkSubShaders();

		VN_CORE_TRACE("[SHADER]({0}) : Compiled Successfully", m_Name);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDetachShader(m_RendererID, m_VertexShader);
		glDetachShader(m_RendererID, m_PixelShader);

		glDeleteShader(m_VertexShader);
		glDeleteShader(m_PixelShader);
		glDeleteShader(m_GeoShader);

		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	
	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::CompileSubShader(uint32_t subShader, const std::string& source)
	{
		int success;
		char logInfo[512];

		const char* cStringSource = source.c_str();
		glShaderSource(subShader, 1, &cStringSource ,nullptr);
		glCompileShader(subShader);

		glGetShaderiv(subShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(subShader, 512, nullptr, logInfo);
			VN_CORE_ERROR("[SHADER]({0}) : Vertex Shader Compilation Failed ... \n {1}", m_Name, logInfo);
		}
	}
	
	void OpenGLShader::LinkSubShaders()
	{
		int success;
		char logInfo[512];

		m_RendererID = glCreateProgram();
		
		glAttachShader(m_RendererID, m_VertexShader);
		glAttachShader(m_RendererID, m_PixelShader);
		
		if (m_GeoShader)
		{
			glAttachShader(m_RendererID, m_GeoShader);
		}

		glLinkProgram(m_RendererID);

		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(m_RendererID, 512, nullptr, logInfo);
			VN_CORE_ERROR("[SHADER]({0}) : Program Linking Failed .. \n {1}", m_Name, logInfo);
		}
	}

	int32_t OpenGLShader::GetUniformLocation(const std::string& name)
	{
		auto it = m_UniformLocations.find(name);

		if (it == m_UniformLocations.end())
		{
			int32_t uniformLocation = glGetUniformLocation(m_RendererID, name.c_str());
			
			if (uniformLocation == -1)
			{
				VN_CORE_WARN("[SHADER]({0}) Uniform {1} doesn't exist", m_Name, name);
			}
			else
			{
				m_UniformLocations.emplace(name, uniformLocation);
			}

			return uniformLocation;
		}
		else
		{
			return it->second;
		}
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int32_t value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}
	
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vec2)
	{
		glUniform2f(GetUniformLocation(name), vec2.x, vec2.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec3) 
	{
		glUniform3f(GetUniformLocation(name), vec3.x, vec3.y, vec3.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vec4)
	{
		glUniform4f(GetUniformLocation(name), vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void OpenGLShader::UploadUniformMatrix3(const std::string& name, const glm::mat3& mat3)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat3));
	}

	void OpenGLShader::UploadUniformMatrix4(const std::string& name, const glm::mat4& mat4)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
	}
}