#include "VulkanCommandBuffer.h"

#include "VulkanContext.h"

namespace Chopper {

	void VulkanCommandBuffer::Allocate(VkCommandPool commandPool, VkCommandBufferLevel level) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = level;
		allocInfo.commandBufferCount = 1;

		VK_MSG_CHECK(
			vkAllocateCommandBuffers(VulkanContext::GetDevice()->Logical(), &allocInfo, &m_CommandBuffer),
			"Failed to allocate Command Buffer"
		);
	}

	void VulkanCommandBuffer::Free(VkCommandPool commandPool) {
		vkFreeCommandBuffers(VulkanContext::GetDevice()->Logical(), commandPool, 1, &m_CommandBuffer);
	}

	void VulkanCommandBuffer::Begin(bool singleUse, bool renderPass, bool simultaneousUse) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		if (singleUse)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (renderPass)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		if (simultaneousUse)
			beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		VK_MSG_CHECK(
			vkBeginCommandBuffer(m_CommandBuffer, &beginInfo),
			"Failed to begin Command Buffer recording"
		);
	}

	void VulkanCommandBuffer::End() {
		vkEndCommandBuffer(m_CommandBuffer);
	}

	void VulkanCommandBuffer::Reset() {
		vkResetCommandBuffer(m_CommandBuffer, 0);
	}

}
