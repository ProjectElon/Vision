#include "pch.hpp"
#include "Logger.h"

namespace Vision
{

	void Logging::Initialize()
	{
		spdlog::set_pattern("%^[%T] %n : %v%$");

		CoreLogger = spdlog::stdout_color_mt("Vision");
		CoreLogger->set_level(spdlog::level::trace);

		ClientLogger = spdlog::stdout_color_mt("Application");
		ClientLogger->set_level(spdlog::level::trace);
	}
	
	void Logging::Shutdown()
	{
	}

	std::shared_ptr<spdlog::logger> Logging::CoreLogger;
	std::shared_ptr<spdlog::logger> Logging::ClientLogger;
}