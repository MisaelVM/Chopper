#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <core/Logger.h>
#include <core/Asserts.h>

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace Chopper {

	class VulkanContext {
	public:
		static VkInstance& GetInstance();
		static VkAllocationCallbacks*& GetAllocator();
		static VkSurfaceKHR& GetSurface();
#ifdef DEBUG_BUILD
		static VkDebugUtilsMessengerEXT& GetDebugMessenger();
#endif
		static VulkanDevice& GetDevice();

		static bool CreateDevice();
		static void ReleaseDevice();

		static void SetFrame(uint32_t frame);

	private:
		static VkInstance s_Instance;
		static VkAllocationCallbacks* s_Allocator;
		static VkSurfaceKHR s_Surface;

#ifdef DEBUG_BUILD
		static VkDebugUtilsMessengerEXT s_DebugMessenger;
#endif
		static VulkanDevice s_Device;

		static VulkanSwapchain s_Swapchain;
		static uint32_t s_ImageIndex;
		static uint32_t s_CurrentFrame;
		static bool s_RecreatingSwapchain;
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
