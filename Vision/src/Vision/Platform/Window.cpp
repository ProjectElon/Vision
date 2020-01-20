#include "pch.h"
#include "Window.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Desktop/DesktopWindow.h"
#endif

namespace Vision
{
	std::unique_ptr<Window> Window::Create(const WindowProps& props)
	{

#ifdef VN_PLATFORM_DESKTOP
		return std::make_unique<DesktopWindow>(props);
#else
		VN_CORE_ASSERT(false, "Unsupported Platform");
		return nullptr;
#endif
	}
}