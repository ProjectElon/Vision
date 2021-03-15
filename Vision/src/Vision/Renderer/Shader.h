#pragma once

#include "Vision/Core/Common.h"
#include "Vision/IO/Assets.h"

#include <glm/glm.hpp>

namespace Vision
{
	struct SubShader
	{
		std::string Source;
		uint32 RendererID = 0;
	};

	struct Shader
	{
		uint32 RendererID = 0;

		std::unordered_map<int32, SubShader> SubShaders;
		std::unordered_map<std::string, int32> UniformLocations;

		Shader() = default;
		Shader(const std::string& filePath);
		~Shader();

		bool Init(const std::string& filePath);
		void Uninit();

		void Bind();
		void Unbind();

		void SetUniformInt(const std::string& uniformName,
						   int32 value);

		void SetUniformIntArray(const std::string& uniformName,
								const int32* values,
								uint32 count);

		void SetUniformFloat(const std::string& uniformName,
							 float32 v0);

		void SetUniformFloat2(const std::string& uniformName,
							  float32 v0,
							  float32 v1);

		void SetUniformFloat3(const std::string& uniformName,
							  float32 v0,
							  float32 v1,
							  float32 v2);

		void SetUniformFloat4(const std::string& uniformName,
							  float32 v0,
							  float32 v1,
							  float32 v2,
							  float32 v3);

		void SetUniformMatrix3(const std::string& uniformName, const float* matrix3);
		void SetUniformMatrix4(const std::string& uniformName, const float* matrix4);
	};

	AssetLoadingData LoadShader(const std::string& shaderpath);
	void UnloadShader(Asset* shader);
}