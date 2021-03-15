#pragma once

#include "Vision/Core/Common.h"

namespace Vision
{
	class Timer
	{
	public:
		float64 ElapsedTime    = 0.0;
		float64 StartTimePoint = 0.0;
		float64 EndTimePoint   = 0.0;

		bool Ticking = false;

		void Start();
		void Stop();
	};
}