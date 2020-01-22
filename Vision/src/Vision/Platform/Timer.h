#pragma once

namespace Vision
{
	class Timer
	{
	public:
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual const double& GetElapsedTime() = 0;
		virtual bool IsTicking() const = 0;

		static std::unique_ptr<Timer> Create();
	};
}