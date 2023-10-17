#include "VulkanImage.h"

#include "VulkanContext.h"

#include <core/Logger.h>

namespace Chopper {

	VulkanImage::VulkanImage(const CustomImageInfo& customImage) {
		Create(customImage);
	}

	VulkanImage::~VulkanImage() {
		Release();
	}

	void VulkanImage::Create(const CustomImageInfo& customImage) {
		VkDevice device = VulkanContext::GetDevice()->Logical();

		m_Width = customImage.Width;
		m_Height = customImage.Height;

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = m_Width;
		imageCreateInfo.extent.height = m_Height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 4;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = customImage.Format;
		imageCreateInfo.tiling = customImage.Tiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = customImage.Usage;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_MSG_CHECK(
			vkCreateImage(device, &imageCreateInfo, VulkanContext::GetAllocator(), &m_Image),
			"Failed to create image!"
		);

		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(device, m_Image, &memRequirements);

		uint32_t memoryType = VulkanContext::FindMemoryType(memRequirements.memoryTypeBits, customImage.MemoryFlags);
		if (memoryType == (uint32_t)-1)
			CHOPPER_LOG_ERROR("Image required an unavailable memory type.");

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = memoryType;

		VK_MSG_CHECK(
			vkAllocateMemory(device, &memAllocInfo, VulkanContext::GetAllocator(), &m_ImageMemory),
			"Failed to allocate image memory!"
		);

		vkBindImageMemory(device, m_Image, m_ImageMemory, 0);

		if (customImage.CreateView)
			CreateImageView(customImage.Format, customImage.ViewAspectFlags);
	}

	void VulkanImage::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = m_Image;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = format;
		viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;

		VK_MSG_CHECK(
			vkCreateImageView(VulkanContext::GetDevice()->Logical(), &viewCreateInfo, VulkanContext::GetAllocator(), &m_ImageView),
			"Failed to create image view!"
		);
	}


	void VulkanImage::Release() {
		VkDevice device = VulkanContext::GetDevice()->Logical();
		VkAllocationCallbacks* allocator = VulkanContext::GetAllocator();

		if (m_ImageView != VK_NULL_HANDLE) {
			vkDestroyImageView(device, m_ImageView, allocator);
			m_ImageView = VK_NULL_HANDLE;
		}

		if (m_ImageMemory != VK_NULL_HANDLE) {
			vkFreeMemory(device, m_ImageMemory, allocator);
			m_ImageMemory = VK_NULL_HANDLE;
		}

		if (m_Image != VK_NULL_HANDLE) {
			vkDestroyImage(device, m_Image, allocator);
			m_Image = VK_NULL_HANDLE;
		}
	}

}
