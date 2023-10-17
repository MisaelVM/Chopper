#include "VulkanDepthImage.h"

#include "VulkanContext.h"

#include <core/Logger.h>

namespace Chopper {

	VulkanDepthImage::VulkanDepthImage(uint32_t width, uint32_t height, VkImageTiling tiling, VkMemoryPropertyFlags properties, bool stencilAttachment) {
		if (VulkanContext::GetInstance() != VK_NULL_HANDLE)
			CreateAttachment(width, height, tiling, properties, stencilAttachment);
	}

	VulkanDepthImage::~VulkanDepthImage() {
		ClearAttachment();
	}

	void VulkanDepthImage::CreateAttachment(uint32_t width, uint32_t height, VkImageTiling tiling, VkMemoryPropertyFlags properties, bool stencilAttachment) {
		VulkanDevice* device = VulkanContext::GetDevice();
		if (device->Logical() == VK_NULL_HANDLE) {
			CHOPPER_LOG_ERROR("Could not create depth attachment! Device handle not valid.");
			return;
		}

		m_Width = width;
		m_Height = height;

		if (m_Width == 0 || m_Height == 0)
			return;

		VkFormat depthFormat = device->GetDepthFormat();
		if (depthFormat == VK_FORMAT_UNDEFINED) {
			CHOPPER_LOG_CRIT("Failed to create depth attachment. Couldn't find a supported depth format!");
			return;
		}

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = m_Width;
		imageCreateInfo.extent.height = m_Height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 4;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = depthFormat;
		imageCreateInfo.tiling = tiling;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_MSG_CHECK(
			vkCreateImage(device->Logical(), &imageCreateInfo, VulkanContext::GetAllocator(), &m_Image),
			"Failed to create depth image!"
		);

		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(device->Logical(), m_Image, &memRequirements);

		uint32_t memoryType = VulkanContext::FindMemoryType(memRequirements.memoryTypeBits, properties);
		if (memoryType == (uint32_t)-1)
			CHOPPER_LOG_ERROR("Depth image required an unavailable memory type.");

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = memoryType;

		VK_MSG_CHECK(
			vkAllocateMemory(device->Logical(), &memAllocInfo, VulkanContext::GetAllocator(), &m_ImageMemory),
			"Failed to allocate depth image memory"
		);

		vkBindImageMemory(device->Logical(), m_Image, m_ImageMemory, 0);

		if (stencilAttachment &&
			depthFormat != VK_FORMAT_D16_UNORM_S8_UINT &&
			depthFormat != VK_FORMAT_D24_UNORM_S8_UINT &&
			depthFormat != VK_FORMAT_D32_SFLOAT_S8_UINT
			) {
			CHOPPER_LOG_ERROR("Depth aspect format supported by device does not support a stencil aspect format.");
			return;
		}

		CreateImageView(depthFormat, stencilAttachment ? (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) : VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanDepthImage::ClearAttachment() {
		Release();
	}
}
