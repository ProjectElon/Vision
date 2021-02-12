#pragma once

#include "Vision/Core/Common.h"
#include "Vision/IO/Assets.h"

#include <glm/glm.hpp>

namespace Vision
{
	struct SubShaderData
	{
		std::string Source;
		int32 RendererID;
	};

	struct Shader
	{
		uint32 RendererID = 0;

		std::unordered_map<int32, SubShaderData> SubShaders;
		std::unordered_map<std::string, int32>	 UniformLocations;
	};

	void CreateShader(Shader* shader, const std::string& filepath);
	void DestroyShader(Shader* shader);

	void BindShader(Shader* shader);
	void UnBindShader(Shader* shader);

	void SetShaderUniformInt(Shader* shader,
							 const std::string& uniformName,
							 int32 value);

	void SetShaderUniformIntArray(Shader* shader,
								  const std::string& uniformName,
								  const int32* values,
								  uint32 count);

	void SetShaderUniformFloat(Shader* shader,
							   const std::string& uniformName,
							   float32 v0);

	void SetShaderUniformFloat2(Shader* shader,
								const std::string& uniformName,
								float32 v0,
								float32 v1);

	void SetShaderUniformFloat3(Shader* shader, 
								const std::string& uniformName,
								float32 v0,
								float32 v1,
								float32 v2);

	void SetShaderUniformFloat4(Shader* shader,
								const std::string& uniformName,
								float32 v0,
								float32 v1,
								float32 v2,
								float32 v3);

	void SetShaderUniformMatrix3(Shader* shader,
								 const std::string& uniformName,
								 const float* matrix3);

	void SetShaderUniformMatrix4(Shader* shader,
								 const std::string& uniformName,
								 const float* matrix4);

	AssetLoadingData LoadShader(const std::string& shaderpath);
	void UnloadShader(Asset* shader);
}