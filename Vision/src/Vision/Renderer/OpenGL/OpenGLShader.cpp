#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"

#ifdef VN_RENDERER_API_OPENGL

#include "Vision/Renderer/Renderer.h"
#include "Vision/Platform/Memory.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"
#include "Vision/Platform/FileSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <fstream>

namespace Vision
{
	static ShaderType StringToShaderType(const std::string& type)
	{
		if (type == "vertex")
		{
			return ShaderType_Vertex;
		}
		else if (type == "pixel" || type == "fragment")
		{
			return ShaderType_Fragment;
		}
		else if (type == "geometry" || type == "geo")
		{
			return ShaderType_Geometry;
		}
		else if (type == "compute" || type == "comp")
		{
			return ShaderType_Compute;
		}

		VnCoreAssert(false, "unsupported shader type: {0}", type);

		return ShaderType_None;
	}

	static GLenum ShaderTypeToOpenGLShaderType(ShaderType type)
	{
		switch (type)
		{
			case ShaderType_Vertex:   return GL_VERTEX_SHADER;
			case ShaderType_Fragment: return GL_FRAGMENT_SHADER;
			case ShaderType_Geometry: return GL_GEOMETRY_SHADER;
			case ShaderType_Compute:  return GL_COMPUTE_SHADER;
		}

		VnCoreAssert(false, "unsupported shader type");
		return ShaderType_None;
	}

	static std::string ShaderTypeToString(ShaderType shaderType)
	{
		switch (shaderType)
		{
			case ShaderType_Vertex:   return "vertex";
			case ShaderType_Fragment: return "fragment";
			case ShaderType_Geometry: return "geometry";
			case ShaderType_Compute:  return "compute";
		}

		VnCoreAssert(false, "unsupported shader type");
		return "none";
	}

	static int32 GetShaderUniformLocation(Shader* shader, const char* uniform)
	{
		int32 uniformIndex = -1;

		for (uint32 index = 0; index < shader->OpenGL.UniformCount; ++index)
		{
			bool isEqual = strcmp(shader->OpenGL.Uniforms[index].Name, uniform) == 0;

			if (isEqual)
			{
				uniformIndex = index;
				break;
			}
		}

		if (uniformIndex == -1)
		{
			int32 uniformLocation = glGetUniformLocation(shader->OpenGL.ProgramHandle, uniform);

			if (uniformLocation == -1)
			{
				VnCoreInfo("opengl: unable to find uniform: {0}", uniform);
			}
			else
			{
				uint32 uinformIndex = shader->OpenGL.UniformCount + 1;
				shader->OpenGL.UniformCount++;
				shader->OpenGL.Uniforms[uinformIndex] = { uniform, uniformLocation };
			}

			return uniformLocation;
		}

		return shader->OpenGL.Uniforms[uniformIndex].Location;
	}

	static void CompileAndAttachShader(Shader* shader,
	                                   ShaderType shaderType,
	                                   const std::string& source)
	{
		uint32* shaderID = &shader->OpenGL.Shaders[shaderType];
		*shaderID = glCreateShader(ShaderTypeToOpenGLShaderType(shaderType));

		const char* cStringSource = source.c_str();
		glShaderSource(*shaderID, 1, &cStringSource, nullptr);
		glCompileShader(*shaderID);

		int32 success;
		char logInfo[512];

		glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &success);

		if (success)
		{
			glAttachShader(shader->OpenGL.ProgramHandle, *shaderID);
		}
		else
		{
			glGetShaderInfoLog(*shaderID, 512, nullptr, logInfo);

			VnCoreInfo("unable to compile {0} shader: {1}",
			           ShaderTypeToString(shaderType),
			           logInfo);

			return;
		}
	}

	void OpenGLInitShader(Shader* shader, const std::string& filePath)
	{
		memset(shader, 0, sizeof(Shader));

		for (uint32 index = 0; index < 128; ++index)
		{
			shader->OpenGL.Uniforms[index].Name	    = "";
			shader->OpenGL.Uniforms[index].Location = -1;
		}

		//@InComplete: Use the engine's file api
		std::ifstream ifs(filePath, std::ios::in);

		if (!ifs.is_open())
		{
			VnCoreInfo("unable to open shader file: {0}", filePath);
			return;
		}

		ShaderType currentShaderType;
		std::string sources[ShaderType_Count];

		std::string line;
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

				currentShaderType = StringToShaderType(typeSignature);
			}
			else
			{
				sources[currentShaderType] += line + '\n';
			}
		}

		ifs.close();

		shader->OpenGL.ProgramHandle = glCreateProgram();

		int32 success;
		char logInfo[512];

		for (uint32 shaderTypeIndex = 0;
		     shaderTypeIndex < ShaderType_Count;
		     ++shaderTypeIndex)
		{
			if (sources[shaderTypeIndex].empty())
			{
				continue;
			}

			CompileAndAttachShader(shader, static_cast<ShaderType>(shaderTypeIndex), sources[shaderTypeIndex]);
		}

		glLinkProgram(shader->OpenGL.ProgramHandle);
		glGetProgramiv(shader->OpenGL.ProgramHandle, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shader->OpenGL.ProgramHandle, sizeof(logInfo), nullptr, logInfo);
			VnCoreInfo("unable to link shader program : {0}", logInfo);
			return;
		}

		return;
	}

	void OpenGLUninitShader(Shader* shader)
	{
		for (uint32 shaderTypeIndex = 0;
		     shaderTypeIndex < ShaderType_Count;
		     ++shaderTypeIndex)
		{
			glDetachShader(shader->OpenGL.ProgramHandle,
			               shader->OpenGL.Shaders[shaderTypeIndex]);
			glDeleteShader(shader->OpenGL.Shaders[shaderTypeIndex]);
			shader->OpenGL.Shaders[shaderTypeIndex] = 0;
		}

		glDeleteProgram(shader->OpenGL.ProgramHandle);
		shader->OpenGL.ProgramHandle = 0;
	}

	void OpenGLBindShader(Shader* shader)
	{
		glUseProgram(shader->OpenGL.ProgramHandle);
	}

	void OpenGLSetUniformInt(Shader* shader,
	                         const char* uniform,
							 int32 value)
	{
		glUniform1i(GetShaderUniformLocation(shader, uniform), value);
	}

	void OpenGLSetUniformIntArray(Shader* shader,
	                              const char* uniform,
								  const int32* values,
								  uint32 count)
	{
		glUniform1iv(GetShaderUniformLocation(shader, uniform), count, values);
	}

	void OpenGLSetUniformFloat(Shader* shader,
	                           const char* uniform,
							   float32 value)
	{
		glUniform1f(GetShaderUniformLocation(shader, uniform), value);
	}

	void OpenGLSetUniformFloat2(Shader* shader,
	                            const char* uniform,
								const float32* values)
	{
		glUniform2f(GetShaderUniformLocation(shader, uniform), values[0], values[1]);
	}

	void OpenGLSetUniformFloat3(Shader* shader,
	                            const char* uniform,
								const float32* values)
	{
		glUniform3f(GetShaderUniformLocation(shader, uniform),
		            values[0],
		            values[1],
		            values[2]);
	}

	void OpenGLSetUniformFloat4(Shader* shader,
	                            const char* uniform,
								const float32* values)
	{
		glUniform4f(GetShaderUniformLocation(shader, uniform),
		            values[0],
		            values[1],
		            values[2],
		            values[3]);
	}

	void OpenGLSetUniformMatrix3(Shader* shader,
	                             const char* uniform,
								 const float* matrix)
	{
		glUniformMatrix3fv(GetShaderUniformLocation(shader, uniform),
		                   1,
		                   GL_FALSE,
		                   matrix);
	}

	void OpenGLSetUniformMatrix4(Shader* shader,
								 const char* uniform,
								 const float* matrix)
	{
		glUniformMatrix4fv(GetShaderUniformLocation(shader, uniform),
		                   1,
		                   GL_FALSE,
		                   matrix);
	}
}

#endif