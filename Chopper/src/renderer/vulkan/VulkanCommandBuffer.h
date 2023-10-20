#pragma once

#include <vulkan/vulkan.hpp>

namespace Chopper {

	class VulkanCommandBuffer {
		friend class VulkanContext;
	public:
		VkCommandBuffer GetHandle() { return m_CommandBuffer; }

	private:
		void Allocate(VkCommandPool commandPool, VkCommandBufferLevel level);
		void Free(VkCommandPool commandPool);

		void Begin(bool singleUse, bool renderPass, bool simultaneousUse);
		void End();
		void Reset();

		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};

}
