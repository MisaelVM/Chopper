#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Logger.h>
#include <core/Asserts.h>

namespace Chopper {

	struct VulkanContext {
		VkInstance Instance = VK_NULL_HANDLE;
		VkAllocationCallbacks* Allocator = nullptr;
#ifdef DEBUG_BUILD
		VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
#endif
	};

}

#ifdef CHOPPER_ASSERTIONS_ENABLED
#define VK_MSG_CHECK(expr, msg)                 \
	{                                           \
		CHOPPER_ASSERT(expr == VK_SUCCESS, msg); \
	}
#else
	{                               \
		VkResult result = expr;     \
		if (result != VK_SUCCESS) { \
			CHOPPER_LOG_ERROR(msg); \
			return false;           \
		}                           \
	}
#endif

#define VK_CHECK(expr) VK_MSG_CHECK(expr, "")
