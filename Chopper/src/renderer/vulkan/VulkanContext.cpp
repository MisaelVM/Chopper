#include "VulkanContext.h"

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

	VkInstance& VulkanContext::GetInstance() { return s_Instance; }
	VkAllocationCallbacks*& VulkanContext::GetAllocator() { return s_Allocator; }
	VkSurfaceKHR& VulkanContext::GetSurface() { return s_Surface; }
#ifdef DEBUG_BUILD
	VkDebugUtilsMessengerEXT& VulkanContext::GetDebugMessenger() { return s_DebugMessenger; }
#endif
	VulkanDevice& VulkanContext::GetDevice() { return s_Device; }

	bool VulkanContext::CreateDevice() { return s_Device.CreateDevice(); }
	void VulkanContext::ReleaseDevice() { s_Device.DestroyDevice(); }

	void VulkanContext::SetFrame(uint32_t frame) { s_CurrentFrame = frame; }

}
