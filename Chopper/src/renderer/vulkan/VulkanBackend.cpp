#pragma once

#include "VulkanBackend.h"

#include "VulkanContext.h"

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Logger.h>
#include <core/Application.h> // TODO: Don't think this is a good thing to do

namespace Chopper {

	static VulkanContext s_Context{};

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

	bool VulkanBackend::Init() {
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

		VK_MSG_CHECK(vkCreateInstance(&instanceInfo, s_Context.Allocator, &s_Context.Instance), "Failed to create instance!");
		CHOPPER_LOG_DEBUG("Vulkan instance successfully created.");

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

		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Context.Instance, "vkCreateDebugUtilsMessengerEXT");
		CHOPPER_ASSERT(func, "Failed to acquire vkCreateDebugUtilsMessengerEXT function!");

		VK_MSG_CHECK(func(s_Context.Instance, &messengerInfo, s_Context.Allocator, &s_Context.DebugMessenger), "Failed to create debug messenger!");
		CHOPPER_LOG_DEBUG("Vulkan debug messenger successfully created.");
#endif

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		VK_MSG_CHECK(glfwCreateWindowSurface(s_Context.Instance, window, s_Context.Allocator, &s_Context.Surface), "Failed to create window surface!");
		CHOPPER_LOG_DEBUG("Vulkan window surface successfully created.");

		s_Context.CreateDevice();

		CHOPPER_LOG_INFO("Vulkan Backend successfully initialized.");
		return true;
	}

	void VulkanBackend::Shutdown() {
		s_Context.DestroyDevice();

		CHOPPER_LOG_DEBUG("Destroying Vulkan Window Surface...");
		vkDestroySurfaceKHR(s_Context.Instance, s_Context.Surface, s_Context.Allocator);
		s_Context.Surface = VK_NULL_HANDLE;
#ifdef DEBUG_BUILD
		CHOPPER_LOG_DEBUG("Destroying Vulkan Debug Messenger...");
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(s_Context.Instance, "vkDestroyDebugUtilsMessengerEXT");
		func(s_Context.Instance, s_Context.DebugMessenger, s_Context.Allocator);
		s_Context.DebugMessenger = VK_NULL_HANDLE;
#endif
		CHOPPER_LOG_DEBUG("Destroying Vulkan Instance...");
		vkDestroyInstance(s_Context.Instance, s_Context.Allocator);
		s_Context.Instance = VK_NULL_HANDLE;
	}

	bool VulkanBackend::BeginFrame(float deltaTime) {
		return true;
	}

	bool VulkanBackend::EndFrame(float deltaTime) {
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
