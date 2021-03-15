#include "pch.h"
#include "Vision/Platform/Timer.h"

#ifdef VN_PLATFORM_DESKTOP

#include <GLFW/glfw3.h>

namespace Vision
{
	void Timer::Start()
	{
		if (!Ticking)
		{
			Ticking = true;
			StartTimePoint = glfwGetTime();
		}
	}

	void Timer::Stop()
	{
		if (Ticking)
		{
			Ticking = false;
			EndTimePoint = glfwGetTime();
			ElapsedTime = EndTimePoint - StartTimePoint;
		}
	}
}

#endif