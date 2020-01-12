#include "pch.h"
#include "Log.h"

namespace Vision
{
	std::shared_ptr< spdlog::logger > Log::s_CoreLogger;
	std::shared_ptr< spdlog::logger > Log::s_ClientLogger;

	void Log::Initialize()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = spdlog::stdout_color_mt("Vision");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("Application");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}