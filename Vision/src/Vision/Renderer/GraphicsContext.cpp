#include "pch.h"
#include "GraphicsContext.h"

#if VN_PLATFORM_DESKTOP
	#include "Vision/Platform/Desktop/OpenGL/DesktopOpenGLContext.h"
#endif

namespace Vision
{
	RenderApi GraphicsContext::s_RenderApi;

	std::unique_ptr<GraphicsContext> GraphicsContext::Create(Window* window)
	{
		switch (GetRenderApi())
		{
			case RenderApi::OpenGL:
			{
				#if VN_PLATFORM_DESKTOP
					return std::make_unique<DesktopOpenGLContext>(window);		
				#endif
			}
			break;
		}
	}
}