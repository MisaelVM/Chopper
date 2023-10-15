#include "VulkanSwapchain.h"

#include "VulkanContext.h"

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Logger.h>
#include <core/Asserts.h>

namespace Chopper {

	VulkanSwapchain::VulkanSwapchain(VulkanContext& context, uint32_t width, uint32_t height)
		: m_Context(context)
	{
		
	}

	VulkanSwapchain::~VulkanSwapchain() {

	}

	bool VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height) {
		auto swapchainSupport = m_Context.Device.QuerySwapchainSupport(m_Context.Device.Physical(), m_Context.Surface, true);

		VkExtent2D extent = { width, height };
		m_MaxFramesInFlight = 2;

		std::vector<VkSurfaceFormatKHR> formats = swapchainSupport.Formats;
		m_SurfaceFormat = formats[0];
		for (const auto& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_SurfaceFormat = format;
				break;
			}
		}

		std::vector<VkPresentModeKHR> presentModes = swapchainSupport.PresentModes;
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& mode : presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				presentMode = mode;
				break;
			}
		}

		VkSurfaceCapabilitiesKHR capabilities = swapchainSupport.Capabilities;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			extent = capabilities.currentExtent;
		}
		else {
			extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		}

		uint32_t imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
			imageCount = capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = m_Context.Surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = m_SurfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		PhysicalDeviceQueueFamilyDetails indices = m_Context.Device.GetQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };
		if (indices.GraphicsFamilyIndex != indices.PresentFamilyIndex) {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}

		swapchainCreateInfo.preTransform = capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = nullptr;

		VK_MSG_CHECK(
			vkCreateSwapchainKHR(m_Context.Device.Logical(), &swapchainCreateInfo, m_Context.Allocator, &m_Swapchain),
			"Failed to create swapchain!"
		);

		m_Context.CurrentFrame = 0;

		imageCount = 0;
		vkGetSwapchainImagesKHR(m_Context.Device.Logical(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Context.Device.Logical(), m_Swapchain, &imageCount, m_SwapchainImages.data());

		m_SwapchainImageViews.resize(imageCount);
		for (uint32_t i = 0; i < imageCount; ++i) {
			VkImageViewCreateInfo imageViewInfo{};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.image = m_SwapchainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = m_SurfaceFormat.format;
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			VK_MSG_CHECK(
				vkCreateImageView(m_Context.Device.Logical(), &imageViewInfo, m_Context.Allocator, &m_SwapchainImageViews[i]),
				"Failed to create Image View!"
			);


		}
	}

	void VulkanSwapchain::DestroySwapchain() {

	}

	bool VulkanSwapchain::RecreateSwapchain() {

	}

	bool VulkanSwapchain::AcquireNextImageIndex(uint64_t timeout, VkSemaphore imageAvailableSem, VkFence fence, uint32_t* pImageIndex) {
		VkResult result = vkAcquireNextImageKHR(m_Context.Device.Logical(), m_Swapchain, timeout, imageAvailableSem, fence, pImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain();
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			CHOPPER_LOG_CRIT("Failed to acquire swapchain image!");
			return false;
		}

		return true;
	}

	void VulkanSwapchain::Present(VkQueue graphicsQueue, VkQueue presentQueue, VkSemaphore renderCompleteSem, uint32_t imageIndex) {
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderCompleteSem;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			RecreateSwapchain();
		else if (result != VK_SUCCESS)
			CHOPPER_LOG_CRIT("Failed to present swapchain image!");
	}
}
