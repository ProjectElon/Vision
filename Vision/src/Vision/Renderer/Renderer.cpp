#include "pch.hpp"

#include "Vision/Core/Application.h"
#include "Vision/Platform/Window.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_RENDERER_API_OPENGL
#include "Vision/Renderer/OpenGL/OpenGLRenderer.h"
#endif 


namespace Vision
{
	void Renderer::Initialize()
	{
		const Settings& settings = Application::Get().Vars.Settings;

#ifdef VN_RENDERER_API_OPENGL

		if (settings.RendererAPI == "OpenGL")
		{
			InitializeOpenGL(&Renderer::API);
		}

#endif
	}

	void Renderer::Shutdown()
	{
		const Settings& settings = Application::Get().Vars.Settings;

#ifdef VN_RENDERER_API_OPENGL

		if (settings.RendererAPI == "OpenGL")
		{
			UninitializeOpenGL();
		}

#endif
	}

    RendererAPI 		Renderer::API;
    ViewportRect 	    Renderer::ViewportRect;
    RendererCapabilites Renderer::Capabilites;
}