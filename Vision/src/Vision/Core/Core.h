#pragma once

/* Platform Detection */

#ifdef _WIN32
	
	#ifdef _WIN64
	
		#define VN_PLATFORM_WINDOWS
	
	#endif

#elif defined(__linux__)
	
	#define VN_PLATFORM_LINUX

#elif defined(__APPLE__) || defined(__MACH__)

	#include <TargetConditionals.h>
	
	#if TARGET_OS_MAC == 1
		
		#define VN_PLATFORM_MACOS
	
	#elif TARGET_OS_IPHONE == 1
		
		#define VN_PLATFORM_IOS
	
	#endif

#elif defined(__ANDROID__)
	
	#define VN_PLATFORM_ANDROID
	
#endif

/* Note : __debugbreak() works only on windows with msvc */

#ifdef VN_DEBUG

	#define VN_CORE_ASSERT(x, ...) { if (!x) { VN_CORE_ERROR("Assertion Failed : {0}", __VA_ARGS__); __debugbreak(); } }
	#define VN_ASSERT(x, ...)      { if (!x) { VN_ERROR("Assertion Failed : {0}", __VA_ARGS__);      __debugbreak(); } }

#else
	
	#define VN_CORE_ASSERT(x, ...)
	#define VN_ASSERT(x, ...)
	
#endif

#define BIT(x) (1 << (x))
#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)