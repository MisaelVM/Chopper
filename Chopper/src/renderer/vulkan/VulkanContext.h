#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Logger.h>
#include <core/Asserts.h>

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace Chopper {

	struct VulkanContext {
		VkInstance Instance = VK_NULL_HANDLE;
		VkAllocationCallbacks* Allocator = nullptr;
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
#ifdef DEBUG_BUILD
		VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
#endif
		VulkanDevice Device{ *this };

		bool CreateDevice() { return Device.CreateDevice(); }
		void DestroyDevice() { Device.DestroyDevice(); }

		VulkanSwapchain Swapchain{ *this };
		uint32_t ImageIndex = 0;
		uint32_t CurrentFrame = 0;
		bool RecreatingSwapchain = false;
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
