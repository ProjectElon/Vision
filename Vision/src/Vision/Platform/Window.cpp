#include "pch.h"
#include "Window.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Desktop/DesktopWindow.h"
#endif

namespace Vision
{
	Scope<Window> Window::Create(const WindowData& props)
	{
	#ifdef VN_PLATFORM_DESKTOP
		return CreateScope<DesktopWindow>(props);
	#endif
	}
}