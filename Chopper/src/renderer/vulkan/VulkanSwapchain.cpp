#include "VulkanSwapchain.h"

#include "VulkanContext.h"

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Logger.h>
#include <core/Asserts.h>

namespace Chopper {

	VulkanSwapchain::VulkanSwapchain(uint32_t width, uint32_t height)
		: m_DepthAttachment(width, height, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		if (VulkanContext::GetDevice()->Logical() != VK_NULL_HANDLE && VulkanContext::GetSurface() != VK_NULL_HANDLE)
			CreateSwapchain(width, height);
	}

	VulkanSwapchain::~VulkanSwapchain() {
		if (VulkanContext::GetDevice()->Logical() != VK_NULL_HANDLE)
			DestroySwapchain();
	}

	bool VulkanSwapchain::CreateSwapchain(uint32_t width, uint32_t height) {
		VulkanDevice* device = VulkanContext::GetDevice();
		auto swapchainSupport = device->QuerySwapchainSupport(device->Physical(), VulkanContext::GetSurface(), true);

		VkExtent2D extent = {width, height};
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
		swapchainCreateInfo.surface = VulkanContext::GetSurface();
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = m_SurfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		PhysicalDeviceQueueFamilyDetails indices = device->GetQueueFamilyIndices();
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
			vkCreateSwapchainKHR(device->Logical(), &swapchainCreateInfo, VulkanContext::GetAllocator(), &m_Swapchain),
			"Failed to create swapchain!"
		);

		VulkanContext::SetFrame(0);

		imageCount = 0;
		vkGetSwapchainImagesKHR(device->Logical(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device->Logical(), m_Swapchain, &imageCount, m_SwapchainImages.data());

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
				vkCreateImageView(device->Logical(), &imageViewInfo, VulkanContext::GetAllocator(), &m_SwapchainImageViews[i]),
				"Failed to create Image View!"
			);
		}

		m_DepthAttachment.CreateAttachment(extent.width, extent.height, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		CHOPPER_LOG_DEBUG("Vulkan Swapchain created successfully.");
		return true;
	}

	void VulkanSwapchain::DestroySwapchain() {
		VkDevice device = VulkanContext::GetDevice()->Logical();
		VkAllocationCallbacks* allocator = VulkanContext::GetAllocator();

		m_DepthAttachment.ClearAttachment();

		for (auto imageView : m_SwapchainImageViews)
			vkDestroyImageView(device, imageView, allocator);

		CHOPPER_LOG_DEBUG("Destroying Vulkan Swapchain...");
		vkDestroySwapchainKHR(device, m_Swapchain, allocator);
		m_Swapchain = VK_NULL_HANDLE;
	}

	bool VulkanSwapchain::RecreateSwapchain(uint32_t width, uint32_t height) {
		DestroySwapchain();
		return CreateSwapchain(width, height);
	}

	bool VulkanSwapchain::AcquireNextImageIndex(uint64_t timeout, VkSemaphore imageAvailableSem, VkFence fence, uint32_t* pImageIndex) {
		VkResult result = vkAcquireNextImageKHR(VulkanContext::GetDevice()->Logical(), m_Swapchain, timeout, imageAvailableSem, fence, pImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapchain(VulkanContext::GetFramebufferWidth(), VulkanContext::GetFramebufferHeight());
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
			RecreateSwapchain(VulkanContext::GetFramebufferWidth(), VulkanContext::GetFramebufferHeight());
		else if (result != VK_SUCCESS)
			CHOPPER_LOG_CRIT("Failed to present swapchain image!");
	}
}
