#pragma once

#include <vulkan/vulkan.hpp>

namespace Chopper {

	class VulkanImage {
	public:
		struct CustomImageInfo {
			VkImageType ImageType;
			uint32_t Width;
			uint32_t Height;
			VkFormat Format;
			VkImageTiling Tiling;
			VkImageUsageFlags Usage;
			VkMemoryPropertyFlags MemoryFlags;
			VkBool32 CreateView;
			VkImageAspectFlags ViewAspectFlags;
		};

		VulkanImage() = default;
		VulkanImage(const CustomImageInfo& customImage);
		virtual ~VulkanImage();


	private:
		void Create(const CustomImageInfo& customImage);

	protected:
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void Release();

		uint32_t m_Width;
		uint32_t m_Height;

		VkImage m_Image = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
	};

}
