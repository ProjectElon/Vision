#pragma once

#include <cstdint>
#include <cassert>
#include <memory>

#include "Vision/Core/Log.h"

/*
-------------------------
	Utilites
--------------------------
*/

#define VnArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

/*
-------------------------
	Types
-------------------------
*/

typedef int8_t   int8;
typedef uint8_t	 uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t	 int32;
typedef uint32_t uint32;
typedef int64_t	 int64;
typedef uint64_t uint64;
typedef float	 float32;
typedef double	 float64;

/*
--------------------------
	Platform Detection
--------------------------
*/

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

/* ---------------------------------------- */

namespace Vision
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

/* Note : __debugbreak() works only on windows with msvc */

#ifdef VN_DEBUG
	
	#define VnCoreAssert(x, ...) { if (!(x)) { VnCoreError(__VA_ARGS__); __debugbreak(); } }
	#define VnAssert(x, ...)     { if (!(x)) { VnError(__VA_ARGS__);     __debugbreak(); } }
		
#else

	#define VnCoreAssert(x, ...)
	#define VnAssert(x, ...) 

#endif

#define VnBitUInt32(x) (1 << (x))
#define VnBitUInt64(x) (1LL << (x))
#define VnBindEventFn(fn) std::bind(&fn, this, std::placeholders::_1)