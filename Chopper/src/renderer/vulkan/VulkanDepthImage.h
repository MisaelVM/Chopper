#pragma once

#include "VulkanImage.h"

namespace Chopper {

	class VulkanDepthImage : public VulkanImage {
	public:
		VulkanDepthImage(uint32_t width, uint32_t height, VkImageTiling tiling, VkMemoryPropertyFlags properties, bool stencilAttachment = false);
		~VulkanDepthImage();

		VkImageView GetView() { return m_ImageView; }

		void CreateAttachment(uint32_t width, uint32_t height, VkImageTiling tiling, VkMemoryPropertyFlags properties, bool stencilAttachment = false);
		void ClearAttachment();
	};

}
