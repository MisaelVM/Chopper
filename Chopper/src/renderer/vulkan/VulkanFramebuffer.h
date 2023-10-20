#pragma once

#include <vulkan/vulkan.hpp>

namespace Chopper {

	class VulkanFramebuffer {
		friend class VulkanContext;
		friend class VulkanSwapchain;
	public:
		VkFramebuffer GetHandle() { return m_Framebuffer; }

	private:
		void Create(uint32_t width, uint32_t height, const std::array<VkImageView, 2>& attachments);
		void Destroy();

		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};

}
