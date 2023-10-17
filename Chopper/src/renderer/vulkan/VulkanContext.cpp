#include "VulkanContext.h"

#include <core/Logger.h>

namespace Chopper {

	VkInstance VulkanContext::s_Instance = VK_NULL_HANDLE;
	VkAllocationCallbacks* VulkanContext::s_Allocator = nullptr;
	VkSurfaceKHR VulkanContext::s_Surface = VK_NULL_HANDLE;
#ifdef DEBUG_BUILD
	VkDebugUtilsMessengerEXT VulkanContext::s_DebugMessenger = VK_NULL_HANDLE;
#endif
	VulkanDevice VulkanContext::s_Device{};
	VulkanSwapchain VulkanContext::s_Swapchain{};
	uint32_t VulkanContext::s_ImageIndex = 0;
	uint32_t VulkanContext::s_CurrentFrame = 0;
	bool VulkanContext::s_RecreatingSwapchain = false;

	uint32_t VulkanContext::s_FramebufferWidth = 0;
	uint32_t VulkanContext::s_FramebufferHeight = 0;

	VkInstance& VulkanContext::GetInstance() { return s_Instance; }
	VkAllocationCallbacks*& VulkanContext::GetAllocator() { return s_Allocator; }
	VkSurfaceKHR& VulkanContext::GetSurface() { return s_Surface; }
#ifdef DEBUG_BUILD
	VkDebugUtilsMessengerEXT& VulkanContext::GetDebugMessenger() { return s_DebugMessenger; }
#endif
	VulkanDevice* VulkanContext::GetDevice() { return &s_Device; }

	bool VulkanContext::CreateDevice() { return s_Device.CreateDevice(); }
	void VulkanContext::ReleaseDevice() { s_Device.DestroyDevice(); }

	bool VulkanContext::CreateSwapchain(uint32_t width, uint32_t height) { return s_Swapchain.CreateSwapchain(width, height); }
	void VulkanContext::DestroySwapchain() { return s_Swapchain.DestroySwapchain(); }
	bool VulkanContext::RecreateSwapchain(uint32_t width, uint32_t height) { return s_Swapchain.RecreateSwapchain(width, height); }

	void VulkanContext::SetFrame(uint32_t frame) { s_CurrentFrame = frame; }
	uint32_t VulkanContext::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties{};
		vkGetPhysicalDeviceMemoryProperties(s_Device.m_PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		CHOPPER_LOG_WARN("Failed to find suitable memory type!");
		return (uint32_t)-1;
	}

	void VulkanContext::SetFramebufferSize(uint32_t width, uint32_t height) { s_FramebufferWidth = width, s_FramebufferHeight = height; }
	
	uint32_t VulkanContext::GetFramebufferWidth() { return s_FramebufferWidth; }
	uint32_t VulkanContext::GetFramebufferHeight() { return s_FramebufferHeight; }

}
