#pragma once

#include "Vision/Renderer/RendererTypes.h"

namespace Vision
{
	void OpenGLInitShader(Shader* shader, const std::string& filePath);
	void OpenGLUninitShader(Shader* shader);

	void OpenGLBindShader(Shader* shader);
	void OpenGLSetUniformInt(Shader* shader, const char* uniform, int32 value);
	void OpenGLSetUniformIntArray(Shader* shader,
		const char* uniform,
		const int32* values,
		uint32 count);

	void OpenGLSetUniformFloat(Shader* shader, const char* uniform, float32 value);

	void OpenGLSetUniformFloat2(Shader* shader,
		const char* uniform,
		const float32* values);
	
	void OpenGLSetUniformFloat3(Shader* shader,
		const char* uniform,
		const float32* values);
	
	void OpenGLSetUniformFloat4(Shader* shader,
		const char* uniform,
		const float32* values);
	
	void OpenGLSetUniformMatrix3(Shader* shader,
		const char* uniform,
		const float32* matrix);
	
	void OpenGLSetUniformMatrix4(Shader* shader,
		const char* uniform,
		const float32* matrix);
}