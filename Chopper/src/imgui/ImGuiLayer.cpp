#include "ImGuiLayer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <core/Application.h>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <renderer/vulkan/VulkanContext.h>
#include <renderer/Renderer.h>

#include <stdlib.h>

namespace Chopper {

	static void check_vk_result(VkResult err) {
		if (err == VK_SUCCESS)
			return;
		CHOPPER_LOG_ERROR("[Dear ImGui Vulkan Backend] Error VkResult = {}", static_cast<int>(err));
		if (err < 0)
			abort();
	}

	void ImGuiLayer::OnAttach() {
		// Setup Dear ImGui Context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = VulkanContext::GetInstance();
		initInfo.PhysicalDevice = VulkanContext::GetDevice()->Physical();
		initInfo.Device = VulkanContext::GetDevice()->Logical();
		initInfo.QueueFamily = VulkanContext::GetDevice()->GetQueueFamilyIndices().GraphicsFamilyIndex;
		initInfo.Queue = VulkanContext::GetDevice()->GetGraphicsQueue();
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.DescriptorPool = VulkanContext::GetDescriptorPool();
		initInfo.Subpass = 0;
		initInfo.MinImageCount = VulkanContext::GetSwapchain()->GetMaxFramesInFlight();
		initInfo.ImageCount = VulkanContext::GetSwapchain()->GetMaxFramesInFlight();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = VulkanContext::GetAllocator();
		initInfo.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&initInfo, VulkanContext::GetRenderPass()->GetHandle());

		// Upload Fonts
		{
			VkResult result;

			// Use any command queue
			VkCommandPool commandPool = VulkanContext::GetDevice()->GetCommandPool();

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			result = vkAllocateCommandBuffers(VulkanContext::GetDevice()->Logical(), &allocInfo, &commandBuffer);
			check_vk_result(result);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
			check_vk_result(result);

			ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

			VkSubmitInfo endInfo{};
			endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			endInfo.commandBufferCount = 1;
			endInfo.pCommandBuffers = &commandBuffer;

			result = vkEndCommandBuffer(commandBuffer);
			check_vk_result(result);
			result = vkQueueSubmit(VulkanContext::GetDevice()->GetGraphicsQueue(), 1, &endInfo, VK_NULL_HANDLE);
			check_vk_result(result);

			result = vkDeviceWaitIdle(VulkanContext::GetDevice()->Logical());
			check_vk_result(result);
			ImGui_ImplVulkan_DestroyFontUploadObjects();

			vkFreeCommandBuffers(VulkanContext::GetDevice()->Logical(), commandPool, 1, &commandBuffer);
		}

	}

	void ImGuiLayer::OnDetach() {
		// Cleanup
		vkDeviceWaitIdle(VulkanContext::GetDevice()->Logical());

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		Renderer::Shutdown();
	}

	void ImGuiLayer::OnImGuiRender() {
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::Begin() {
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImDrawData* mainDrawData = ImGui::GetDrawData();
		const bool main_is_minimized = (mainDrawData->DisplaySize.x <= 0.0f || mainDrawData->DisplaySize.y <= 0.0f);

		RenderData data{};
		data.DeltaTime = 0.0f;
		data.ImGuiDrawData = mainDrawData;

		bool beginFrameSuccess = false;
		if (!main_is_minimized)
			beginFrameSuccess = Renderer::BeginFrame(&data);

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Present Main Platform Window
		if (!main_is_minimized && beginFrameSuccess) {
			bool result = Renderer::EndFrame(&data);
			CHOPPER_ASSERT(result, "Failed to draw frame!");
		}
	}

}
