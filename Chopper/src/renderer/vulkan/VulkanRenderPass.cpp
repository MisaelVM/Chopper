#include "VulkanRenderPass.h"

#include "VulkanContext.h"

#include <common/includes.h>

namespace Chopper {

	void VulkanRenderPass::CreateRenderPass(
		VkRect2D renderArea,
		VkClearColorValue clearColor,
		float depth, int stencil,
		bool addDepthAtt
	) {
		if (m_RenderPass != VK_NULL_HANDLE)
			return;

		m_RenderArea = renderArea;
		m_ClearColor = clearColor;
		m_Depth = depth;
		m_Stencil = stencil;

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VulkanContext::GetSwapchain()->GetSurfaceFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		VkAttachmentReference depthAttachmentRef{};
		if (addDepthAtt) {
			depthAttachment.format = VulkanContext::GetDevice()->GetDepthFormat();
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		if (addDepthAtt) {
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
		}
		// TODO: Maybe in a later version
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.pResolveAttachments = nullptr;
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		// dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // TODO: Temporary. Just for the sake of ImGui integration
		std::vector<VkAttachmentDescription> attachments = { colorAttachment };
		if (addDepthAtt) {
			attachments.push_back(depthAttachment);
		}

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCreateInfo.pAttachments = attachments.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		VK_MSG_CHECK(
			vkCreateRenderPass(VulkanContext::GetDevice()->Logical(), &renderPassCreateInfo, VulkanContext::GetAllocator(), &m_RenderPass),
			"Failed to create Vulkan Render Pass!"
		);
		CHOPPER_LOG_DEBUG("Vulkan Render Pass successfully created.");
	}

	void VulkanRenderPass::ReleaseRenderPass() {
		if (m_RenderPass == VK_NULL_HANDLE)
			return;

		CHOPPER_LOG_DEBUG("Destroying Vulkan Render Pass...");
		vkDestroyRenderPass(VulkanContext::GetDevice()->Logical(), m_RenderPass, VulkanContext::GetAllocator());
		m_RenderPass = VK_NULL_HANDLE;
	}

	void VulkanRenderPass::Begin(VkFramebuffer framebuffer) {
		VkRenderPassBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = m_RenderPass;
		beginInfo.framebuffer = framebuffer;
		beginInfo.renderArea = m_RenderArea;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = m_ClearColor;
		clearValues[1].depthStencil.depth = m_Depth;
		clearValues[1].depthStencil.stencil = m_Stencil;

		beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		beginInfo.pClearValues = clearValues.data();

		VkCommandBuffer commandBuffer = VulkanContext::GetCurrentCommandBuffer();
		vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End() {
		VkCommandBuffer commandBuffer = VulkanContext::GetCurrentCommandBuffer();
		vkCmdEndRenderPass(commandBuffer);
	}

}
