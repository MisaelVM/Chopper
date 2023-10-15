#pragma once

#include <vulkan/vulkan.hpp>

namespace Chopper {

	struct VulkanSwapchain {
		friend class VulkanContext;
	public:
		VulkanSwapchain(VulkanContext& context, uint32_t width = 0, uint32_t height = 0);
		~VulkanSwapchain();

		bool AcquireNextImageIndex(uint64_t timeout, VkSemaphore imageAvailableSem, VkFence fence, uint32_t* pImageIndex);
		void Present(VkQueue graphicsQueue, VkQueue presentQueue, VkSemaphore renderCompleteSem, uint32_t imageIndex);

	private:
		bool CreateSwapchain(uint32_t width, uint32_t height);
		void DestroySwapchain();
		bool RecreateSwapchain();

		VkSurfaceFormatKHR m_SurfaceFormat{};
		uint8_t m_MaxFramesInFlight = 2;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;

		VulkanContext& m_Context;
	};

}
