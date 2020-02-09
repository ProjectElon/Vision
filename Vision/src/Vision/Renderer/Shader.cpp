#include "pch.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLShader.h"
#endif

namespace Vision
{
	uint32_t Shader::GetDataTypeSize(DataType dataType)
	{
		switch (dataType)
		{
			case DataType::Bool: { return 1; } break;

			case DataType::Int:  { return 1 * 4; } break;
			case DataType::Int2: { return 2 * 4; } break;
			case DataType::Int3: { return 3 * 4; } break;
			case DataType::Int4: { return 4 * 4; } break;

			case DataType::UInt: { return 1 * 4; } break;
			case DataType::UInt2: { return 2 * 4; } break;
			case DataType::UInt3: { return 3 * 4; } break;
			case DataType::UInt4: { return 4 * 4; } break;

			case DataType::Float:  { return 1 * 4; } break;
			case DataType::Float2: { return 2 * 4; } break;
			case DataType::Float3: { return 3 * 4; } break;
			case DataType::Float4: { return 4 * 4; } break;
			
			case DataType::Matrix3: { return 3 * 3 * 4; } break;
			case DataType::Matrix4: { return 4 * 4 * 4; } break;
		}
	}

	uint32_t Shader::GetComponentCount(DataType dataType)
	{
		switch (dataType)
		{
			case DataType::Bool: { return 1; } break;

			case DataType::Int:  { return 1; } break;
			case DataType::Int2: { return 2; } break;
			case DataType::Int3: { return 3; } break;
			case DataType::Int4: { return 4; } break;

			case DataType::Float:  { return 1; } break;
			case DataType::Float2: { return 2; } break;
			case DataType::Float3: { return 3; } break;
			case DataType::Float4: { return 4; } break;

			case DataType::Matrix3: { return 3 * 3; } break;
			case DataType::Matrix4: { return 4 * 4; } break;
		}
	}

	Shader* Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return new OpenGLShader(name, vertexSource, fragmentSource);
			}
			break;
		}
	}
}