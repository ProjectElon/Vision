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
	static int32 GetShaderUniformLocation(Shader* shader, const std::string& uniformName)
	{
		auto it = shader->UniformLocations.find(uniformName);

		if (it == shader->UniformLocations.end())
		{
			int32 uniformLocation = glGetUniformLocation(shader->RendererID, uniformName.c_str());

			if (uniformLocation == -1)
			{
				// Warn Message
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

	static void CompileAndLinkSubShaders(Shader* shader)
	{
		shader->RendererID = glCreateProgram();

		int32 success;
		char logInfo[512];

		for (auto& subShader : shader->SubShaders)
		{
			auto& [type, data] = subShader;
			data.RendererID = glCreateShader(type);

			const char* cStringSource = data.Source.c_str();
			glShaderSource(data.RendererID, 1, &cStringSource, nullptr);
			glCompileShader(data.RendererID);

			glGetShaderiv(data.RendererID, GL_COMPILE_STATUS, &success);

			if (success)
			{
				glAttachShader(shader->RendererID, data.RendererID);
			}
			else
			{
				glGetShaderInfoLog(data.RendererID, 512, nullptr, logInfo);
			}
		}

		glLinkProgram(shader->RendererID);
		glGetProgramiv(shader->RendererID, GL_LINK_STATUS, &success);

		if (success)
		{
			VnCoreInfo("Shader Compiled");
		}
		else
		{
			glGetProgramInfoLog(shader->RendererID, 512, nullptr, logInfo);
		}
	}

	static GLenum GetShaderTypeFromString(const std::string& type)
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
	}

	void CreateShader(Shader* shader, const std::string& filepath)
	{
		//@(Harlequin): Use the engine's file api

		std::ifstream ifs(filepath, std::ios::in);

		if (!ifs.is_open())
		{
			return;
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

				currentShaderType = GetShaderTypeFromString(typeSignature);
			}
			else
			{
				shader->SubShaders[currentShaderType].Source += line + '\n';
			}
		}

		ifs.close();

		CompileAndLinkSubShaders(shader);
	}

	void DestroyShader(Shader* shader)
	{
		for (const auto& subShader : shader->SubShaders)
		{
			const auto& [type, data] = subShader;
			glDetachShader(shader->RendererID, data.RendererID);
			glDeleteShader(data.RendererID);
		}

		glDeleteProgram(shader->RendererID);
		shader->SubShaders.clear();
	}

	void BindShader(Shader* shader)
	{
		glUseProgram(shader->RendererID);
	}

	void UnBindShader(Shader* shader)
	{
		glUseProgram(0);
	}

	void SetShaderUniformInt(Shader* shader,
							 const std::string& uniformName,
							 int32 value)
	{
		glUniform1i(GetShaderUniformLocation(shader, uniformName), value);
	}

	void SetShaderUniformIntArray(Shader* shader,
								  const std::string& uniformName,
								  const int32* values,
								  uint32 count)
	{
		glUniform1iv(GetShaderUniformLocation(shader, uniformName), count, values);
	}

	void SetShaderUniformFloat(Shader* shader,
							   const std::string& uniformName,
							   float32 v0)
	{
		glUniform1f(GetShaderUniformLocation(shader, uniformName), v0);
	}

	void SetShaderUniformFloat2(Shader* shader,
								const std::string& uniformName,
								float32 v0,
								float32 v1)
	{
		glUniform2f(GetShaderUniformLocation(shader, uniformName), v0, v1);
	}

	void SetShaderUniformFloat3(Shader* shader,
								const std::string& uniformName,
								float32 v0,
								float32 v1,
								float32 v2)
	{
		glUniform3f(GetShaderUniformLocation(shader, uniformName), v0, v1, v2);
	}

	void SetShaderUniformFloat4(Shader* shader,
								const std::string& uniformName,
								float32 v0,
								float32 v1,
								float32 v2,
								float32 v3)
	{
		glUniform4f(GetShaderUniformLocation(shader, uniformName), v0, v1, v2, v3);
	}

	void SetShaderUniformMatrix3(Shader* shader,
								 const std::string& uniformName,
								 const float* matrix3)
	{
		glUniformMatrix3fv(GetShaderUniformLocation(shader, uniformName), 1, GL_FALSE, matrix3);
	}

	void SetShaderUniformMatrix4(Shader* shader,
								 const std::string& uniformName,
								 const float* matrix4)
	{
		glUniformMatrix4fv(GetShaderUniformLocation(shader, uniformName), 1, GL_FALSE, matrix4);
	}

	AssetLoadingData LoadShader(const std::string& shaderpath)
	{
		AssetLoadingData shaderAsset;

		Shader* shader = new Shader;

		CreateShader(shader, shaderpath);

		shaderAsset.Memory = shader;
		shaderAsset.SizeInBytes = sizeof(Shader);
		shaderAsset.TotalSizeInBytes = sizeof(Shader);

		return shaderAsset;
	}

	void UnloadShader(Asset* shaderAsset)
	{
		Shader* shader = (Shader*)shaderAsset->Memory;
		DestroyShader(shader);
		delete shader;
	}
}