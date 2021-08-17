#pragma once

#define VN_ENABLE_LOGGING 1
#define VN_ENABLE_ASSERTIONS 1

#include <cstdint>

#define VnArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))

#define VnBitU32Mask(bitIndex) (1   << (bitIndex))
#define VnBitU64Mask(bitIndex) (1LL << (bitIndex))
#define VnBindEventFn(fn) std::bind(&fn, this, std::placeholders::_1)

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t	 int32;
typedef int64_t	 int64;

typedef uint8_t	 uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  float32;
typedef double float64;

typedef size_t memorysize;

#ifdef _WIN32

	#ifdef _WIN64

		#define VN_PLATFORM_WINDOWS

		#define VN_RENDERER_API_OPENGL
		#define VN_RENDERER_API_DIRECTX
		#define VN_RENDERER_API_VULKAN

	#endif

#elif defined(__linux__)

	#define VN_PLATFORM_LINUX

	#define VN_RENDERER_API_OPENGL
	#define VN_RENDERER_API_VULKAN

#elif defined(__APPLE__) || defined(__MACH__)

	#include <TargetConditionals.h>

	#if TARGET_OS_MAC == 1

		#define VN_PLATFORM_MACOS

		#define VN_RENDERER_API_METAL
		#define VN_RENDERER_API_VULKAN


	#elif TARGET_OS_IPHONE == 1

		#define VN_PLATFORM_IOS

		#define VN_RENDERER_API_METAL
		#define VN_RENDERER_API_VULKAN

	#endif

#elif defined(__ANDROID__)

	#define VN_PLATFORM_ANDROID

	#define VN_RENDERER_API_OPENGL
	#define VN_RENDERER_API_VULKAN

#endif

#define introspect_struct(category, handleSwitchCase)
#define ignore_methods()