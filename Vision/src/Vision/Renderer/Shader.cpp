#include "pch.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Renderer/OpenGL/OpenGLShader.h"
#endif

namespace Vision
{
	Ref<Shader> Shader::CreateFromFile(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				return CreateRef<OpenGLShader>(filepath);
			}
			break;
		}
	}
}