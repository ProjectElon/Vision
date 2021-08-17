#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

#include "Vision/Core/Defines.h"

namespace Vision
{
	struct Logging
	{
		static std::shared_ptr<spdlog::logger> CoreLogger;
		static std::shared_ptr<spdlog::logger> ClientLogger;

		static void Initialize();
		static void Shutdown();
	};
}

#if VN_ENABLE_LOGGING && !defined(VN_DIST)
	
#define VnCoreTrace(...)    Vision::Logging::CoreLogger->trace(__VA_ARGS__)
#define VnCoreInfo(...)     Vision::Logging::CoreLogger->info(__VA_ARGS__)
#define VnCoreWarn(...)     Vision::Logging::CoreLogger->warn(__VA_ARGS__)
#define VnCoreError(...)    Vision::Logging::CoreLogger->error(__VA_ARGS__)
#define VnCoreFetal(...)	Vision::Logging::CoreLogger->fetal(__VA_ARGS__)
#define VnCoreCritical(...) Vision::Logging::CoreLogger->critical(__VA_ARGS__)

#define VnTrace(...)    Vision::Logging::ClientLogger->trace(__VA_ARGS__)
#define VnInfo(...)     Vision::Logging::ClientLogger->info(__VA_ARGS__)
#define VnWarn(...)     Vision::Logging::ClientLogger->warn(__VA_ARGS__)
#define VnError(...)    Vision::Logging::ClientLogger->error(__VA_ARGS__)
#define VnFetal(...)    Vision::Logging::ClientLogger->fetal(__VA_ARGS__)
#define VnCritical(...) Vision::Logging::ClientLogger->critical(__VA_ARGS__)

#else

#define VnCoreTrace(...)
#define VnCoreInfo(...)
#define VnCoreWarn(...)
#define VnCoreError(...)
#define VnCoreFetal(...)
#define VnCoreCritical(...)

#define VnTrace(...)
#define VnInfo(...)
#define VnWarn(...)
#define VnError(...)
#define VnFetal(...)
#define VnCritical(...)
	
#endif

#if _MSC_VER

#include <intrin.h>
#define VnDebugBreak() __debugbreak()

#else

#define VnDebugBreak() __builtin_trap()

#endif

#if VN_ENABLE_ASSERTIONS && !defined(VN_DIST)

#define VnCoreAssert(x, ...) { \
		if (!(x)) { \
			VnCoreError("assert: {0} at file: {1} line: {2}", #x, __FILE__, __LINE__); \
			VnDebugBreak(); \
		} \
	} \

#define VnAssert(x, ...) { \
		if (!(x)) { \
			VnError("assert: {0} at file: {1} line: {2}", #x, __FILE__, __LINE__); \
			VnDebugBreak(); \
		} \
	} \

#else

#define VnCoreAssert(x, ...)
#define VnAssert(x, ...)

#endif