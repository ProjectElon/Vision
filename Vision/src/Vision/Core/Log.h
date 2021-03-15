#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace Vision
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger()
		{ 
			return CoreLogger;
		}

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger()
		{ 
			return ClientLogger;
		}

	private:
		static std::shared_ptr<spdlog::logger> CoreLogger;
		static std::shared_ptr<spdlog::logger> ClientLogger;
	};
}

#ifndef VN_DIST
	
	#define VnCoreTrace(...)    Vision::Log::GetCoreLogger()->trace(__VA_ARGS__)
	#define VnCoreInfo(...)     Vision::Log::GetCoreLogger()->info(__VA_ARGS__)
	#define VnCoreWarn(...)     Vision::Log::GetCoreLogger()->warn(__VA_ARGS__)
	#define VnCoreError(...)    Vision::Log::GetCoreLogger()->error(__VA_ARGS__)
	#define VnCoreCritical(...) Vision::Log::GetCoreLogger()->critical(__VA_ARGS__)
	
	#define VnTrace(...)    Vision::Log::GetClientLogger()->trace(__VA_ARGS__)
	#define VnInfo(...)     Vision::Log::GetClientLogger()->info(__VA_ARGS__)
	#define VnWarn(...)     Vision::Log::GetClientLogger()->warn(__VA_ARGS__)
	#define VnError(...)    Vision::Log::GetClientLogger()->error(__VA_ARGS__)
	#define VnCritical(...) Vision::Log::GetClientLogger()->critical(__VA_ARGS__)

#else

	#define VN_CORE_TRACE(...)
	#define VN_CORE_INFO(...)
	#define VN_CORE_WARN(...)
	#define VN_CORE_ERROR(...)
	#define VN_CORE_FATAL(...)
	#define VN_CORE_CRITICAL(...)

	#define VN_TRACE(...) 
	#define VN_INFO(...)  
	#define VN_WARN(...)
	#define VN_ERROR(...)
	#define VN_FATAL(...)
	#define VN_CRITICAL(...)
	
#endif