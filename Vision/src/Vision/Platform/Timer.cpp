#include "pch.h"
#include "Timer.h"

#ifdef VN_PLATFORM_DESKTOP
	#include "Vision/Platform/Desktop/DesktopTimer.h"
#endif

namespace Vision
{
	Scope<Timer> Timer::Create()
	{
	#ifdef VN_PLATFORM_DESKTOP
		return MakeScope<DesktopTimer>();
	#endif
	}
}