#pragma once

#include "Vision/Platform/Timer.h"

namespace Vision
{
	class DesktopTimer : public Timer
	{
	public:
		DesktopTimer();

		void Start() override;
		void Stop() override;
		inline const double& GetElapsedTime() override { return m_ElapsedTime; }
		inline bool IsTicking() const override { return m_Ticking; }

	private:
		double m_StartTimePoint;
		double m_EndTimePoint;
		double m_ElapsedTime;
		bool m_Ticking;
	};
}