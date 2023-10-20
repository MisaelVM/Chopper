#pragma once

#include <vulkan/vulkan.hpp>

namespace Chopper {

	class VulkanRenderPass {
		friend class VulkanContext;
	public:
		VkRenderPass GetHandle() { return m_RenderPass; }

		void SetRenderArea(VkRect2D renderArea) { m_RenderArea = renderArea; }
		void SetClearColor(VkClearColorValue clearColor) { m_ClearColor = clearColor; }

		void Begin(VkFramebuffer framebuffer);
		void End();

	private:
		void CreateRenderPass(
			VkRect2D renderArea,
			VkClearColorValue clearColor,
			float depth, int stencil
		);
		void ReleaseRenderPass();

		VkRect2D m_RenderArea;
		VkClearColorValue m_ClearColor;

		float m_Depth;
		int m_Stencil;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};

}
