#include "pch.h"
#include "DesktopTimer.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	DesktopTimer::DesktopTimer()
		: m_StartTimePoint(0.0)
		, m_EndTimePoint(0.0)
		, m_ElapsedTime(0.0)
		, m_Ticking(false)
	{
	}

	void DesktopTimer::Start()
	{
		if (m_Ticking == false)
		{
			m_Ticking = true;
			m_StartTimePoint = glfwGetTime();
		}
	}
	
	void DesktopTimer::Stop() 
	{
		if (m_Ticking)
		{
			m_Ticking = false;
			m_EndTimePoint = glfwGetTime();
			m_ElapsedTime = m_EndTimePoint - m_StartTimePoint;
		}
	}
}