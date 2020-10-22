#include "pch.h"
#include "Vision/Platform/Timer.h"

#ifdef VN_PLATFORM_DESKTOP

#include <GLFW/glfw3.h>

namespace Vision
{
	void Timer::Start()
	{
		if (Ticking == false)
		{
			Ticking = true;
			m_StartTimePoint = glfwGetTime();
		}
	}
	
	void Timer::Stop()
	{
		if (Ticking)
		{
			Ticking = false;
			m_EndTimePoint = glfwGetTime();
			ElapsedTime = m_EndTimePoint - m_StartTimePoint;
		}
	}
}

#endif