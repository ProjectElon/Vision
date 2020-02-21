#include "pch.h"
#include "GraphicsContext.h"
#include "Renderer.h"

#if VN_PLATFORM_DESKTOP
	#include "Vision/Platform/Desktop/OpenGL/DesktopOpenGLContext.h"
#endif

namespace Vision
{
	Scope<GraphicsContext> GraphicsContext::Create(Window* window)
	{
		switch (Renderer::GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				#if VN_PLATFORM_DESKTOP
					return MakeScope<DesktopOpenGLContext>(window);		
				#endif
			}
			break;
		}
	}
}