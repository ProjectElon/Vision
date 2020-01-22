#include "pch.h"
#include "Timer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Platform/Desktop/DesktopTimer.h"
#endif

namespace Vision
{
	std::unique_ptr<Timer> Timer::Create()
	{
	#ifdef VN_PLATFORM_DESKTOP
		return std::unique_ptr<Timer>(new DesktopTimer);
	#endif
	}
}