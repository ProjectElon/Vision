#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Vision
{
	class Log
	{
	public:
		static void Initialize();

		inline static std::shared_ptr< spdlog::logger >& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr< spdlog::logger >& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr< spdlog::logger > s_CoreLogger;
		static std::shared_ptr< spdlog::logger > s_ClientLogger;
	};
}

#ifndef VN_DIST

#define VN_CORE_TRACE(...) Vision::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VN_CORE_INFO(...)  Vision::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VN_CORE_WARN(...)  Vision::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VN_CORE_ERROR(...) Vision::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VN_CORE_FATAL(...) Vision::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define VN_TRACE(...) Vision::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VN_INFO(...)  Vision::Log::GetClientLogger)->info(__VA_ARGS__)
#define VN_WARN(...)  Vision::Log::GetClientLogger)->warn(__VA_ARGS__)
#define VN_ERROR(...) Vision::Log::GetClientLogger()->error(__VA_ARGS__)
#define VN_FATAL(...) Vision::Log::GetClientLogger()->fatal(__VA_ARGS__)

#else

#define VN_CORE_TRACE(...)  Vision::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VN_CORE_INFO(...)   Vision::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VN_CORE_WARN(...)   Vision::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VN_CORE_ERROR(...)  Vision::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VN_CORE_FATAL(...)  Vision::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define VN_TRACE(...)  Vision::Log::GetClientLogger()->trace(__VA_ARGS__)
#define VN_INFO(...)   Vision::Log::GetClientLogger)->info(__VA_ARGS__)
#define VN_WARN(...)   Vision::Log::GetClientLogger)->warn(__VA_ARGS__)
#define VN_ERROR(...)  Vision::Log::GetClientLogger()->error(__VA_ARGS__)
#define VN_FATAL(...)  Vision::Log::GetClientLogger()->fatal(__VA_ARGS__)

#endif
