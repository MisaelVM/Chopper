#include "VulkanFramebuffer.h"

#include "VulkanContext.h"

#include <common/includes.h>

namespace Chopper {

	void VulkanFramebuffer::Create(uint32_t width, uint32_t height, const std::array<VkImageView, 2>& attachments) {
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = VulkanContext::GetRenderPass()->GetHandle();
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1;

		VK_MSG_CHECK(
			vkCreateFramebuffer(VulkanContext::GetDevice()->Logical(), &framebufferCreateInfo, VulkanContext::GetAllocator(), &m_Framebuffer),
			"Failed to create Vulkan Framebuffer"
		)
	}

	void VulkanFramebuffer::Destroy() {
		vkDestroyFramebuffer(VulkanContext::GetDevice()->Logical(), m_Framebuffer, VulkanContext::GetAllocator());
		m_Framebuffer = VK_NULL_HANDLE;
	}

}
