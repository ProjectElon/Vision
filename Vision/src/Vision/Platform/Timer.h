#pragma once

#include "Vision/Core/Common.h"

namespace Vision
{
	class Timer
	{
	public:
		double ElapsedTime = 0.0;
		bool Ticking = false;

		void Start();
		void Stop();
		
	private:
		double m_StartTimePoint = 0.0;
		double m_EndTimePoint = 0.0;
	};
}