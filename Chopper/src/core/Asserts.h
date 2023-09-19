#pragma once

#include <common/definitions.h>

#define CHOPPER_ASSERTIONS_ENABLED

#ifdef CHOPPER_ASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define DebugBreak() __debugbreak();
#else
#define DebugBreak() __builtin_trap();
#endif
#define CHOPPER_ASSERT(expr, msg) \
	{                        \
		if (expr) {}         \
		else {               \
			CHOPPER_LOG_ERROR("Assertion Failure: {0} in file {1}, line {2}. {3}", #expr, __FILE__, __LINE__, msg); \
			DebugBreak();    \
		}                    \
	}
#else
#define CHOPPER_ASSERT(expr)
#endif
