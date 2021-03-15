#include "pch.h"
#include "Log.h"

namespace Vision
{
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n : %v%$");

		CoreLogger = spdlog::stdout_color_mt("Vision");
		CoreLogger->set_level(spdlog::level::trace);

		ClientLogger = spdlog::stdout_color_mt("Application");
		ClientLogger->set_level(spdlog::level::trace);
	}
	
	std::shared_ptr<spdlog::logger> Log::CoreLogger;
	std::shared_ptr<spdlog::logger> Log::ClientLogger;
}