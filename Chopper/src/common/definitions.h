#pragma once

// Platform detection

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	#define CHOPPER_WINDOWS_PLATFORM
	#ifndef _WIN64
		#error "64-bit Windows is required!"
	#endif // !_WIN64
#elif defined(__linux__)
	#define CHOPPER_LINUX_PLATFORM
	#ifdef __ANDROID__
		#define CHOPPER_ANDROID_PLATFORM
	#endif // __ANDROID__
#else
	#error "Unknown platform!"
#endif

// Shared libraries export/import

#ifdef CHOPPER_WINDOWS_PLATFORM
	#ifdef BUILD_LIBS
		#define CHOPPER_API __declspec(dllexport)
	#else
		#define CHOPPER_API __declspec(dllimport)
	#endif // BUILD_LIBS
#elif defined(CHOPPER_LINUX_PLATFORM)
	#ifdef BUILD_LIBS
		#define CHOPPER_API __attribute__((visibility("default")))
	#else
		#define CHOPPER_API
	#endif // BUILD_LIBS
#else
	#error "Unsupported or unknown platform!"
#endif

// Aditional includes

#include <cstdint>
