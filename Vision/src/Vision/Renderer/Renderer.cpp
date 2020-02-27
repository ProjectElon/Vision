#include "pch.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"

#include "Vision/Renderer/OpenGL/OpenGLRendererAPI.h"
#include "Vision/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	Renderer::API Renderer::s_API = API::None;
	Scope<RendererAPI> Renderer::s_RendererAPI;
	
	void Renderer::Init()
	{
		switch (GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				s_RendererAPI = CreateScope<OpenGLRendererAPI>();
			}
			break;
		}

		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
		s_RendererAPI.release();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->SetViewport(0, 0, width, height);
	}

}