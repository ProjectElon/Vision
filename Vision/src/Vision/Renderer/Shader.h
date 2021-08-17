#pragma once

#include "Vision/Core/Defines.h"

#include <glm/glm.hpp>

namespace Vision
{
	enum ShaderType : uint32
	{
		ShaderType_Vertex   = 0,
		ShaderType_Fragment = 1,
		ShaderType_Geometry = 2,
		ShaderType_Compute  = 3,
		ShaderType_Count    = 4,
		ShaderType_None     = 5
	};

#ifdef VN_RENDERER_API_OPENGL

	#define MaxUniformLocations 128

	struct OpenGLUniform
	{
		const char* Name = "";
		int32 Location   = -1;
	};

#endif

	struct Shader
	{
		union
		{
#ifdef VN_RENDERER_API_OPENGL
			struct
			{
				uint32 ProgramID;
				uint32 Shaders[ShaderType_Count];
				uint32 UniformCount;
				OpenGLUniform Uniforms[MaxUniformLocations];
			}
			OpenGL;
#endif
		};
	};

#ifdef VN_RENDERER_API_OPENGL

		void OpenGLInitShader(Shader* shader, const std::string& filePath);
		void OpenGLUninitShader(Shader* shader);

		void OpenGLBindShader(Shader* shader);
		void OpenGLSetUniformInt(Shader* shader, const char* uniform, int32 value);
		void OpenGLSetUniformIntArray(Shader* shader,
		                              const char* uniform,
		                              const int32* values,
		                              uint32 count);

		void OpenGLSetUniformFloat(Shader*  shader, const char* uniform, float32 value);
		void OpenGLSetUniformFloat2(Shader* shader,
		                            const char* uniform,
		                            const float32* values);
		void OpenGLSetUniformFloat3(Shader*  shader,
		                            const char* uniform,
		                            const float32* values);
		void OpenGLSetUniformFloat4(Shader*  shader,
		                            const char* uniform,
		                            const float32* values);
		void OpenGLSetUniformMatrix3(Shader* shader,
		                             const char* uniform,
		                             const float32* matrix);
		void OpenGLSetUniformMatrix4(Shader* shader,
		                             const char* uniform,
		                             const float32* matrix);

#endif
}
