#include "pch.h"
#include "Vision/Core/Log.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/IO/FileSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <fstream>

namespace Vision
{
	static GLenum StringToOpenGLShaderType(const std::string& type)
	{
		if (type == "vertex")
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

		VnCoreAssert(false, "unsupported shader type: {0}", type);
	}

	static std::string OpenGLShaderTypeToString(GLenum shaderType)
	{
		switch (shaderType)
		{
			case GL_VERTEX_SHADER: 	 return "vertex";
			case GL_FRAGMENT_SHADER: return "pixel";
			case GL_GEOMETRY_SHADER: return "geometry";
			case GL_COMPUTE_SHADER:  return "compute";
		}

		VnCoreAssert(false, "unsupported shader type");
	}

	static int32 GetShaderUniformLocation(Shader* shader, const std::string& uniformName)
	{
		auto it = shader->UniformLocations.find(uniformName);

		if (it == shader->UniformLocations.end())
		{
			int32 uniformLocation = glGetUniformLocation(shader->RendererID, uniformName.c_str());

			if (uniformLocation == -1)
			{
				VnCoreInfo("opengl: unable to find uniform: {0}", uniformName);
			}
			else
			{
				shader->UniformLocations.emplace(uniformName, uniformLocation);
			}

			return uniformLocation;
		}
		else
		{
			return it->second;
		}
	}

	Shader::Shader(const std::string& filePath)
	{
		Init(filePath);
	}

	Shader::~Shader()
	{
		Uninit();
	}

	bool Shader::Init(const std::string& filePath)
	{
		//@(Harlequin): Use the engine's file api

		std::ifstream ifs(filePath, std::ios::in);

		if (!ifs.is_open())
		{
			VnCoreInfo("unable to open shader file: {0}", filePath);
			return false;
		}

		std::string line;
		GLenum currentShaderType;

		while (std::getline(ifs, line))
		{
			line.erase(0, line.find_first_not_of(" \t\n\r"));
			line.erase(line.find_last_not_of(" \t\n\r") + 1);

			if (line.empty())
			{
				continue;
			}
			else if (line.front() == '#' && line.find("#type") == 0)
			{
				std::string typeSignature = line.substr(5); // 5 => #type

				typeSignature.erase(0, typeSignature.find_first_not_of(" \t\n"));
				typeSignature.erase(typeSignature.find_last_not_of(" \t\n") + 1);

				currentShaderType = StringToOpenGLShaderType(typeSignature);
			}
			else
			{
				SubShaders[currentShaderType].Source += line + '\n';
			}
		}

		ifs.close();

		RendererID = glCreateProgram();

		int32 success;
		char logInfo[512];

		for (auto& subShader : SubShaders)
		{
			auto& [type, data] = subShader;
			data.RendererID = glCreateShader(type);

			const char* cStringSource = data.Source.c_str();
			glShaderSource(data.RendererID, 1, &cStringSource, nullptr);
			glCompileShader(data.RendererID);

			glGetShaderiv(data.RendererID, GL_COMPILE_STATUS, &success);

			if (success)
			{
				glAttachShader(RendererID, data.RendererID);
			}
			else
			{
				glGetShaderInfoLog(data.RendererID, 512, nullptr, logInfo);

				VnCoreInfo("unable to compile {0} shader in shader file: {1}\n{2}",
				           OpenGLShaderTypeToString(type),
				           filePath,
				           logInfo);

				return false;
			}
		}

		glLinkProgram(RendererID);
		glGetProgramiv(RendererID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(RendererID, 512, nullptr, logInfo);
			VnCoreInfo("unable to link shader program : {0}", logInfo);
			return false;
		}

		return true;
	}

	void Shader::Uninit()
	{
		for (const auto& subShader : SubShaders)
		{
			const auto& [type, data] = subShader;
			glDetachShader(RendererID, data.RendererID);
			glDeleteShader(data.RendererID);
		}

		glDeleteProgram(RendererID);
		SubShaders.clear();
	}

	void Shader::Bind()
	{
		glUseProgram(RendererID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::SetUniformInt(const std::string& uniformName,
							   int32 value)
	{
		glUniform1i(GetShaderUniformLocation(this, uniformName), value);
	}

	void Shader::SetUniformIntArray(const std::string& uniformName,
								    const int32* values,
								    uint32 count)
	{
		glUniform1iv(GetShaderUniformLocation(this, uniformName), count, values);
	}

	void Shader::SetUniformFloat(const std::string& uniformName,
							     float32 v0)
	{
		glUniform1f(GetShaderUniformLocation(this, uniformName), v0);
	}

	void Shader::SetUniformFloat2(const std::string& uniformName,
								  float32 v0,
								  float32 v1)
	{
		glUniform2f(GetShaderUniformLocation(this, uniformName), v0, v1);
	}

	void Shader::SetUniformFloat3(const std::string& uniformName,
								  float32 v0,
								  float32 v1,
								  float32 v2)
	{
		glUniform3f(GetShaderUniformLocation(this, uniformName), v0, v1, v2);
	}

	void Shader::SetUniformFloat4(const std::string& uniformName,
								  float32 v0,
								  float32 v1,
								  float32 v2,
								  float32 v3)
	{
		glUniform4f(GetShaderUniformLocation(this, uniformName), v0, v1, v2, v3);
	}

	void Shader::SetUniformMatrix3(const std::string& uniformName,
								   const float* matrix3)
	{
		glUniformMatrix3fv(GetShaderUniformLocation(this, uniformName),
		                   1,
		                   GL_FALSE,
		                   matrix3);
	}

	void Shader::SetUniformMatrix4(const std::string& uniformName,
								   const float* matrix4)
	{
		glUniformMatrix4fv(GetShaderUniformLocation(this, uniformName),
		                   1,
		                   GL_FALSE,
		                   matrix4);
	}

	AssetLoadingData LoadShader(const std::string& filePath)
	{
		AssetLoadingData shaderAsset;

		Shader* shader = new Shader;

		if (shader->Init(filePath))
		{
			shaderAsset.Memory = shader;
			shaderAsset.SizeInBytes = sizeof(Shader);
			shaderAsset.TotalSizeInBytes = sizeof(Shader);
		}
		else
		{
			delete shader;
		}

		return shaderAsset;
	}

	void UnloadShader(Asset* shaderAsset)
	{
		Shader* shader = (Shader*)shaderAsset->Memory;
		delete shader;
	}
}