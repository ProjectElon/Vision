#include "pch.h"
#include "Renderer.h"

#include "Vision/Renderer/OpenGL/OpenGLRendererAPI.h"

namespace Vision
{
	RendererAPI* Renderer::s_RendererAPI = nullptr;
	Renderer::API Renderer::s_API = API::None;

	void Renderer::Initialize()
	{
		switch (GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				s_RendererAPI = new OpenGLRendererAPI;
			}
			break;
		}
	}

	void Renderer::Shutdown()
	{
		delete s_RendererAPI;
	}
}