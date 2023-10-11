#pragma once

#include "VulkanBackend.h"

#include "VulkanContext.h"

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Logger.h>

namespace Chopper {

	static VulkanContext s_Context{};

	VulkanBackend::VulkanBackend() {
		if (!Init())
			CHOPPER_LOG_CRIT("Vulkan Backend could not be initialized!");
	}

	VulkanBackend::~VulkanBackend() {}

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
		}

		VkInstanceCreateInfo instanceInfo{};
		instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceInfo.pApplicationInfo = &appInfo;
		instanceInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		instanceInfo.ppEnabledExtensionNames = requiredExtensions.data();
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = nullptr;

		VK_MSG_CHECK(vkCreateInstance(&instanceInfo, s_Context.Allocator, &s_Context.Instance), "Failed to create instance!");
		CHOPPER_LOG_INFO("Vulkan instance successfully created.");

		CHOPPER_LOG_INFO("Vulkan Backend successfully initialized.");
		return true;
	}

	bool VulkanBackend::BeginFrame(float deltaTime) {
		return true;
	}

	bool VulkanBackend::EndFrame(float deltaTime) {
		return true;
	}

}
