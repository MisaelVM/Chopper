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
	VulkanRenderPass VulkanContext::s_RenderPass{};
	std::vector<VulkanCommandBuffer> VulkanContext::s_CommandBuffers{};
	std::vector<VkSemaphore> VulkanContext::s_ImageAvailableSemaphores{};
	std::vector<VkSemaphore> VulkanContext::s_RenderFinishedSemaphores{};
	std::vector<VkFence> VulkanContext::s_InFlightFences{};

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
	VulkanSwapchain* VulkanContext::GetSwapchain() { return &s_Swapchain; }
	VulkanRenderPass* VulkanContext::GetRenderPass() { return &s_RenderPass; }

	bool VulkanContext::CreateDevice() { return s_Device.CreateDevice(); }
	void VulkanContext::ReleaseDevice() { s_Device.DestroyDevice(); }

	bool VulkanContext::CreateSwapchain(uint32_t width, uint32_t height) { return s_Swapchain.CreateSwapchain(width, height); }
	void VulkanContext::DestroySwapchain() { return s_Swapchain.DestroySwapchain(); }
	bool VulkanContext::RecreateSwapchain(uint32_t width, uint32_t height) { return s_Swapchain.RecreateSwapchain(width, height); }

	void VulkanContext::CreateRenderPass(VkRect2D renderArea, VkClearColorValue clearColor, float depth, int stencil) {
		s_RenderPass.CreateRenderPass(renderArea, clearColor, depth, stencil);
	}
	void VulkanContext::ReleaseRenderPass() { s_RenderPass.ReleaseRenderPass(); }

	void VulkanContext::CreateSyncObjects() {
		size_t maxFramesInFlight = s_Swapchain.m_MaxFramesInFlight;

		s_ImageAvailableSemaphores.resize(maxFramesInFlight);
		s_RenderFinishedSemaphores.resize(maxFramesInFlight);
		s_InFlightFences.resize(maxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < maxFramesInFlight; ++i) {
			vkCreateSemaphore(s_Device.m_LogicalDevice, &semaphoreCreateInfo, s_Allocator, &s_ImageAvailableSemaphores[i]);
			vkCreateSemaphore(s_Device.m_LogicalDevice, &semaphoreCreateInfo, s_Allocator, &s_RenderFinishedSemaphores[i]);
			vkCreateFence(s_Device.m_LogicalDevice, &fenceCreateInfo, s_Allocator, &s_InFlightFences[i]);
		}
		CHOPPER_LOG_DEBUG("Vulkan Synchronization Objects created sucessfully.");
	}

	void VulkanContext::ReleaseSyncObjtects() {
		CHOPPER_LOG_DEBUG("Destroying Vulkan Synchronization Objects...");
		for (size_t i = 0; i < s_Swapchain.m_MaxFramesInFlight; ++i) {
			vkDestroyFence(s_Device.m_LogicalDevice, s_InFlightFences[i], s_Allocator);
			vkDestroySemaphore(s_Device.m_LogicalDevice, s_RenderFinishedSemaphores[i], s_Allocator);
			vkDestroySemaphore(s_Device.m_LogicalDevice, s_ImageAvailableSemaphores[i], s_Allocator);
		}
	}

	void VulkanContext::SetupCommandBuffers() {
		uint32_t count = s_Swapchain.m_SwapchainImages.size();
		s_CommandBuffers.resize(count);

		for (auto& commandBuffer : s_CommandBuffers) {
			commandBuffer.Free(s_Device.m_CommandPool);
			commandBuffer.Allocate(s_Device.m_CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		}
	}

	VkCommandBuffer VulkanContext::GetCurrentCommandBuffer(bool begin) {
		if (begin) {
			s_CommandBuffers[s_ImageIndex].Reset();
			s_CommandBuffers[s_ImageIndex].Begin(false, false, false);
		}
		return s_CommandBuffers[s_ImageIndex].m_CommandBuffer;
	}
	void VulkanContext::BeginCurrentCommandBuffer() {
		s_CommandBuffers[s_ImageIndex].Reset();
		s_CommandBuffers[s_ImageIndex].Begin(false, false, false);
	}
	void VulkanContext::EndCurrentCommandBuffer() {
		s_CommandBuffers[s_ImageIndex].End();
	}

	void VulkanContext::NextFrame() { s_CurrentFrame = (s_CurrentFrame + 1) % s_Swapchain.m_MaxFramesInFlight; }
	void VulkanContext::ResetFrameIndex() { s_CurrentFrame = 0; }
	void VulkanContext::SetFrameIndex(uint32_t frame) { s_CurrentFrame = frame; }
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
	
	uint32_t VulkanContext::GetImageIndex() { return s_ImageIndex; }
	void VulkanContext::SetImageIndex(uint32_t imageIndex) { s_ImageIndex = imageIndex; }

	bool VulkanContext::IsSwapchainRecreating() { return s_RecreatingSwapchain; }
	void VulkanContext::SetSwapchainRecreating(bool recreate) { s_RecreatingSwapchain = recreate; }

	VkFramebuffer VulkanContext::GetCurrentFramebuffer() { return s_Swapchain.m_Framebuffers[s_ImageIndex].GetHandle(); }
	VkSemaphore VulkanContext::GetCurrentImageAvailableSemaphore() { return s_ImageAvailableSemaphores[s_CurrentFrame]; }
	VkSemaphore VulkanContext::GetCurrentRenderFinishedSemaphore() { return s_RenderFinishedSemaphores[s_CurrentFrame]; }
	VkFence VulkanContext::GetCurrentInFlightFence() { return s_InFlightFences[s_CurrentFrame]; }

	uint32_t VulkanContext::GetFramebufferWidth() { return s_FramebufferWidth; }
	uint32_t VulkanContext::GetFramebufferHeight() { return s_FramebufferHeight; }

}
