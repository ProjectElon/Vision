#include "pch.h"
#include "Vision/Core/Log.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"
#include "Vision/Utils/StringUtils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Vision
{
	OpenGLShader::OpenGLShader(const std::string& filePath)
		: m_RendererID(0)
	{
		size_t lastSlash = filePath.find_last_of("/\\");
		size_t lastDot = filePath.rfind('.');

		size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
		size_t count = (lastDot == std::string::npos) ? filePath.length() - start : lastDot - start;

		m_Name = filePath.substr(start, count);

		std::ifstream ifs(filePath, std::ios::in | std::ios::binary);

		if (!ifs.is_open())
		{
			VN_CORE_ERROR("[SHADER]({0}) Unable to open file : {1}", m_Name, filePath);
			return;
		}

		std::string line;
		GLenum currentShaderType;

		while (std::getline(ifs, line))
		{
			StringUtils::Trim(line);

			if (line.empty())
			{
				continue;
			}
			else if (line.front() == '#' && StringUtils::StartsWith(line, "#type"))
			{
				std::string typeSignature = line.substr(5);
				StringUtils::Trim(typeSignature);
				currentShaderType = GetShaderTypeFromString(typeSignature);
			}
			else 
			{
				m_Shaders[currentShaderType].Source += line + '\n';
			}
		}

		for (auto& shader : m_Shaders)
		{
			auto& [type, data] = shader;
			data.RendererID = glCreateShader(type);
			CompileShader(data);
		}

		LinkShaders();

		VN_CORE_TRACE("[SHADER]({0}) Loaded Sucessfully", m_Name);
	}

	OpenGLShader::~OpenGLShader()
	{
		for (const auto& shader : m_Shaders)
		{
			const auto& [type, data] = shader;
			glDetachShader(m_RendererID, data.RendererID);
			glDeleteShader(data.RendererID);
		}

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

	void OpenGLShader::SetInt(const std::string& name, int32_t value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& vec2)
	{
		glUniform2f(GetUniformLocation(name), vec2.x, vec2.y);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& vec3)
	{
		glUniform3f(GetUniformLocation(name), vec3.x, vec3.y, vec3.z);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& vec4)
	{
		glUniform4f(GetUniformLocation(name), vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void OpenGLShader::SetMatrix3(const std::string& name, const glm::mat3& mat3)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat3));
	}

	void OpenGLShader::SetMatrix4(const std::string& name, const glm::mat4& mat4)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void OpenGLShader::CompileShader(const ShaderData& shaderData)
	{
		int success;
		char logInfo[512];

		const char* cStringSource = shaderData.Source.c_str();
		glShaderSource(shaderData.RendererID, 1, &cStringSource, nullptr);
		glCompileShader(shaderData.RendererID);

		glGetShaderiv(shaderData.RendererID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shaderData.RendererID, 512, nullptr, logInfo);
			VN_CORE_ERROR("[SHADER]({0}) : Vertex Shader Compilation Failed ... \n {1}", m_Name, logInfo);
		}
	}
	
	void OpenGLShader::LinkShaders()
	{
		int success;
		char logInfo[512];

		m_RendererID = glCreateProgram();

		for (const auto& shader : m_Shaders)
		{
			const auto& [type, data] = shader;
			glAttachShader(m_RendererID, data.RendererID);
		}

		glLinkProgram(m_RendererID);
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(m_RendererID, 512, nullptr, logInfo);
			VN_CORE_ERROR("[SHADER]({0}) : Program Linking Failed .. \n {1}", m_Name, logInfo);
		}
	}

	GLenum OpenGLShader::GetShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex" || type == "point")
		{
			return GL_VERTEX_SHADER;
		}
		else if (type == "pixel" || type == "fragment")
		{
			return GL_FRAGMENT_SHADER;
		}
		else if (type == "geo" || type == "geometry")
		{
			return GL_GEOMETRY_SHADER;
		}
		else if (type == "compute")
		{
			return GL_COMPUTE_SHADER;
		}

		VN_CORE_ASSERT(false, "[SHADER]({0}) Type {1} is not supported", m_Name, type);
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

	GLenum OpenGLShader::GetGLTypeFromShaderType(Shader::DataType dataType)
	{
		switch (dataType)
		{
			case DataType::Bool:  { return GL_BOOL;          } break;
			case DataType::Byte:  { return GL_BYTE;          } break;
			case DataType::UByte: { return GL_UNSIGNED_BYTE; } break;

			case DataType::Short:  { return GL_SHORT;          } break;
			case DataType::UShort: { return GL_UNSIGNED_SHORT; } break;

			case DataType::Int:  { return GL_INT; } break;
			case DataType::Int2: { return GL_INT; } break;
			case DataType::Int3: { return GL_INT; } break;
			case DataType::Int4: { return GL_INT; } break;
			
			case DataType::UInt:  { return GL_UNSIGNED_INT; } break;
			case DataType::UInt2: { return GL_UNSIGNED_INT; } break;
			case DataType::UInt3: { return GL_UNSIGNED_INT; } break;
			case DataType::UInt4: { return GL_UNSIGNED_INT; } break;

			case DataType::Float:  { return GL_FLOAT; } break;
			case DataType::Float2: { return GL_FLOAT; } break;
			case DataType::Float3: { return GL_FLOAT; } break;
			case DataType::Float4: { return GL_FLOAT; } break;

			case DataType::Matrix3: { return GL_FLOAT; } break;
			case DataType::Matrix4: { return GL_FLOAT; } break;
		}
	}
}