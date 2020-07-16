#pragma once

#include "Vision/Core/Base.h"

namespace Vision
{
	class Timer
	{
	public:
		virtual void Start() = 0;
		virtual void Stop() = 0;
		
		virtual const double& GetElapsedTime() = 0;
		virtual bool IsTicking() const = 0;

		static Scope<Timer> Create();
	};
}