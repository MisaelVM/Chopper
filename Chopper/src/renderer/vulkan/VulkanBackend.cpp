#pragma once

#include "VulkanBackend.h"

#include "VulkanContext.h"

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Logger.h>
#include <core/Application.h> // TODO: Don't think this is a good thing to do

namespace Chopper {

	// static VulkanContext s_Context{};

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

	VulkanBackend::VulkanBackend() {
		if (!Init())
			CHOPPER_LOG_CRIT("Vulkan Backend could not be initialized!");
	}

	VulkanBackend::~VulkanBackend() {
		Shutdown();
	}

	// TODO: Maybe all of this logic must be inside VulkanContext and just be invoked form here
	bool VulkanBackend::Init() {
		VkInstance& instance = VulkanContext::GetInstance();
		VkAllocationCallbacks*& allocator = VulkanContext::GetAllocator();
		VkSurfaceKHR& surface = VulkanContext::GetSurface();

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Chopper Engine Testbed"; // TODO: Make this configurable
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0); // TODO: Make this configurable
		appInfo.pEngineName = "Chopper Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0); // TODO: Make this configurable
		appInfo.apiVersion = VK_API_VERSION_1_3;

		// TODO: This depends on GLFW3, if you're planning on dropping GLFW you should refactor
		// the way you get the required extensions for vulkan instancing
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#ifdef DEBUG_BUILD
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif

		{
			// Checking extension support
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

			std::unordered_set<std::string> unsupportedExtensions(requiredExtensions.begin(), requiredExtensions.end());
			for (const auto& extension : availableExtensions) {
				std::string extensionName = extension.extensionName;
				if (unsupportedExtensions.count(extensionName))
					unsupportedExtensions.erase(extensionName);
			}

			if (unsupportedExtensions.size()) {
				CHOPPER_LOG_ERROR("Some instance extensions are not supported!");
				// TODO: Maybe log which extensions are not supported
				return false;
			}

#ifdef DEBUG_BUILD
			// Checking layer support
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			std::unordered_set<std::string> unsupportedLayers(validationLayers.begin(), validationLayers.end());
			for (const auto& layer : availableLayers) {
				std::string layerName = layer.layerName;
				if (unsupportedLayers.count(layerName))
					unsupportedLayers.erase(layerName);
			}

			if (unsupportedLayers.size()) {
				CHOPPER_LOG_ERROR("Some instance layers are not supported!");
				// TODO: Maybe log which layers are not supported
				return false;
			}
#endif
		}

		VkInstanceCreateInfo instanceInfo{};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();
#ifdef DEBUG_BUILD
		instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		instanceInfo.ppEnabledLayerNames = validationLayers.data();
#else
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;
#endif

		VK_MSG_CHECK(
			vkCreateInstance(&instanceInfo, allocator, &instance),
			"Failed to create Vulkan Instance!"
		);
		CHOPPER_LOG_DEBUG("Vulkan Instance created successfully.");

#ifdef DEBUG_BUILD
		CHOPPER_LOG_INFO("Validation layers are enabled. Creating Vulkan Debugger...");

		VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
		messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		messengerInfo.pfnUserCallback = DebugCallback;
		messengerInfo.pUserData = nullptr;

		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		CHOPPER_ASSERT(func, "Failed to acquire vkCreateDebugUtilsMessengerEXT function!");

		VK_MSG_CHECK(
			func(instance, &messengerInfo, allocator, &VulkanContext::GetDebugMessenger()),
			"Failed to create Vulkan Debug Messenger!"
		);
		CHOPPER_LOG_DEBUG("Vulkan Debug Messenger created successfully.");
#endif

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		VK_MSG_CHECK(
			glfwCreateWindowSurface(instance, window, allocator, &surface),
			"Failed to create Window Surface!"
		);
		CHOPPER_LOG_DEBUG("Vulkan Window Surface created successfully.");

		VulkanContext::CreateDevice();

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		VulkanContext::SetFramebufferSize(w, h);
		VulkanContext::CreateSwapchain(w, h);

		VkRect2D renderArea{};
		renderArea.offset = { 0, 0 };
		renderArea.extent = { VulkanContext::GetFramebufferWidth(), VulkanContext::GetFramebufferHeight() };
		VkClearColorValue clearColor{};
		clearColor.float32[0] = 0.0f;
		clearColor.float32[1] = 0.0f;
		clearColor.float32[2] = 0.2f;
		clearColor.float32[3] = 1.0f;
		VulkanContext::CreateRenderPass(renderArea, clearColor, 1.0f, 0);

		VulkanContext::GetSwapchain()->RegenerateFramebuffers();

		VulkanContext::SetupCommandBuffers();

		VulkanContext::CreateSyncObjects();

		CHOPPER_LOG_INFO("Vulkan Backend initialized successfully.");
		return true;
	}

	void VulkanBackend::Shutdown() {
		VkInstance& instance = VulkanContext::GetInstance();
		VkAllocationCallbacks*& allocator = VulkanContext::GetAllocator();
		VkSurfaceKHR& surface = VulkanContext::GetSurface();
		
		vkDeviceWaitIdle(VulkanContext::GetDevice()->Logical());

		VulkanContext::ReleaseSyncObjtects();
		VulkanContext::ReleaseRenderPass();
		VulkanContext::DestroySwapchain();
		VulkanContext::ReleaseDevice();

		CHOPPER_LOG_DEBUG("Destroying Vulkan Window Surface...");
		vkDestroySurfaceKHR(instance, surface, allocator);
		surface = VK_NULL_HANDLE;
#ifdef DEBUG_BUILD
		VkDebugUtilsMessengerEXT messenger = VulkanContext::GetDebugMessenger();
		CHOPPER_LOG_DEBUG("Destroying Vulkan Debug Messenger...");
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		func(instance, messenger, allocator);
		messenger = VK_NULL_HANDLE;
#endif
		CHOPPER_LOG_DEBUG("Destroying Vulkan Instance...");
		vkDestroyInstance(instance, allocator);
		instance = VK_NULL_HANDLE;
	}

	bool VulkanBackend::BeginFrame(float deltaTime) {
		VkDevice device = VulkanContext::GetDevice()->Logical();

		if (VulkanContext::IsSwapchainRecreating()) {
			VkResult result = vkDeviceWaitIdle(device);
			if (result != VK_SUCCESS) {
				CHOPPER_LOG_ERROR("VulkanBackend::BeginFrame() failed!");
				return false;
			}
			CHOPPER_LOG_INFO("Recreating swapchain.");
			return false;
		}

		VkFence fence = VulkanContext::GetCurrentInFlightFence();
		VkFence waitFences[] = { fence };

		VK_MSG_CHECK(
			vkWaitForFences(device, 1, waitFences, VK_TRUE, UINT64_MAX),
			"InFlightFence had a wait failure!"
		);

		VkSemaphore imageAvailableSemaphore = VulkanContext::GetCurrentImageAvailableSemaphore();

		uint32_t imageIndex = VulkanContext::GetImageIndex();
		if (!VulkanContext::GetSwapchain()->AcquireNextImageIndex(
			UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex)
		) {
			CHOPPER_LOG_WARN("Failed to acquire next image.");
			return false;
		}
		VulkanContext::SetImageIndex(imageIndex);

		vkResetFences(device, 1, waitFences);

		VkCommandBuffer commandBuffer = VulkanContext::GetCurrentCommandBuffer(true);

		float framebufferWidth = static_cast<float>(VulkanContext::GetFramebufferWidth());
		float framebufferHeight = static_cast<float>(VulkanContext::GetFramebufferHeight());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = framebufferHeight;
		viewport.width = framebufferWidth;
		viewport.height = -framebufferHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { static_cast<uint32_t>(framebufferWidth), static_cast<uint32_t>(framebufferHeight) };

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkRect2D renderArea{};
		renderArea.offset = { 0, 0 };
		renderArea.extent = { VulkanContext::GetFramebufferWidth(), VulkanContext::GetFramebufferHeight() };
		VulkanContext::GetRenderPass()->SetRenderArea(renderArea);
		VulkanContext::GetRenderPass()->Begin(VulkanContext::GetCurrentFramebuffer());

		return true;
	}

	bool VulkanBackend::EndFrame(float deltaTime) {
		VulkanContext::GetRenderPass()->End();

		VulkanContext::EndCurrentCommandBuffer();

		VkCommandBuffer commandBuffer[] = { VulkanContext::GetCurrentCommandBuffer() };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffer;
		
		VkSemaphore signalSemaphores[] = { VulkanContext::GetCurrentRenderFinishedSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkSemaphore waitSemaphores[] = { VulkanContext::GetCurrentImageAvailableSemaphore() };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.pWaitDstStageMask = waitStages;

		VkQueue graphicsQueue = VulkanContext::GetDevice()->GetGraphicsQueue();
		VkQueue presentQueue = VulkanContext::GetDevice()->GetPresentQueue();

		VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VulkanContext::GetCurrentInFlightFence());
		if (result != VK_SUCCESS) {
			CHOPPER_LOG_ERROR("Failed to submit queue!");
			return false;
		}

		VulkanContext::GetSwapchain()->Present(
			graphicsQueue, presentQueue,
			VulkanContext::GetCurrentRenderFinishedSemaphore(),
			VulkanContext::GetImageIndex()
		);

		return true;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			CHOPPER_LOG_TRACE(pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			CHOPPER_LOG_INFO(pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			CHOPPER_LOG_WARN(pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			CHOPPER_LOG_ERROR(pCallbackData->pMessage);
			break;
		default:
			break;
		}

		return VK_FALSE;
	}

}
