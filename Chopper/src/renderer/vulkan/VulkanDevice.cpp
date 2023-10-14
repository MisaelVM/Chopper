#include "VulkanDevice.h"

#include "VulkanContext.h"

#include <common/definitions.h>
#include <common/includes.h>

#include <core/Logger.h>
#include <core/Asserts.h>

namespace Chopper {

#define CHECK_QUEUE_SUPPORT(requirement, queueFamIndex, queue)                             \
	{                                                                                      \
		if (requirement && queueFamIndex == (uint32_t)-1)                                  \
		{                                                                                  \
			CHOPPER_LOG_INFO("Candidate device does not satisfy {} requirements!", queue); \
			return false;                                                                  \
		}                                                                                  \
	}

	VulkanDevice::VulkanDevice(VulkanContext& context)
		: m_ContextInstance(context.Instance), m_ContextSurface(context.Surface), m_ContextAllocators(context.Allocator)
	{
		if (m_ContextInstance != VK_NULL_HANDLE && m_ContextSurface != VK_NULL_HANDLE) {
			CreateDevice();
		}
	}

	VulkanDevice::~VulkanDevice() {
		if (m_ContextInstance != VK_NULL_HANDLE && m_LogicalDevice != VK_NULL_HANDLE)
			DestroyDevice();
	}

	bool VulkanDevice::CreateDevice() {
		if (!PickPhysicalDevice())
			return false;

		std::set<uint32_t> uniqueQueueFamilies = {
			m_QueueFamilyIndices.GraphicsFamilyIndex,
			m_QueueFamilyIndices.PresentFamilyIndex,
			m_QueueFamilyIndices.TransferFamilyIndex,
			// m_QueueFamilyIndices.ComputeFamilyIndex // No support for compute queues yet
		};

		std::vector<uint32_t> uniqueIndices(uniqueQueueFamilies.begin(), uniqueQueueFamilies.end());
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());

		float queuePriority = 1.0f;
		float queuePriorities[2] = { 1.0f, 1.0f };
		for (uint32_t i = 0; i < queueCreateInfos.size(); ++i) {
			queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos[i].queueFamilyIndex = uniqueIndices[i];
			if (uniqueIndices[i] == m_QueueFamilyIndices.GraphicsFamilyIndex) {
				queueCreateInfos[i].queueCount = 2;
				queueCreateInfos[i].pQueuePriorities = queuePriorities;
			}
			else {
				queueCreateInfos[i].queueCount = 1;
				queueCreateInfos[i].pQueuePriorities = &queuePriority;
			}
			queueCreateInfos[i].flags = 0;
			queueCreateInfos[i].pNext = nullptr;
		}

		// TODO: This should follow requirement's specifications.
		//       Assuming SamplerAnisotropy is a requirement, at the moment
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = 1;

		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		
		VK_MSG_CHECK(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, m_ContextAllocators, &m_LogicalDevice), "Failed to create Vulkan Logical Device!");
		CHOPPER_LOG_DEBUG("Vulkan Logical Device successfully created.");

		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.GraphicsFamilyIndex, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.PresentFamilyIndex, 0, &m_PresentQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilyIndices.TransferFamilyIndex, 0, &m_TransferQueue);

		return true;
	}

	void VulkanDevice::DestroyDevice() {
		m_TransferQueue = VK_NULL_HANDLE;
		m_PresentQueue = VK_NULL_HANDLE;
		m_GraphicsQueue = VK_NULL_HANDLE;

		CHOPPER_LOG_DEBUG("Destroying Vulkan Logical Device...");
		vkDestroyDevice(m_LogicalDevice, m_ContextAllocators);
		m_LogicalDevice = VK_NULL_HANDLE;

		m_PhysicalDevice = VK_NULL_HANDLE;
	}

	bool VulkanDevice::PickPhysicalDevice() {
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(m_ContextInstance, &physicalDeviceCount, nullptr);

		if (!physicalDeviceCount) {
			CHOPPER_LOG_CRIT("No devices with Vulkan support found!");
			return false;
		}

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(m_ContextInstance, &physicalDeviceCount, physicalDevices.data());

		for (const auto& physicalDevice : physicalDevices) {
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(physicalDevice, &properties);

			VkPhysicalDeviceFeatures features{};
			vkGetPhysicalDeviceFeatures(physicalDevice, &features);

			VkPhysicalDeviceMemoryProperties memory{};
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory);


			// TODO: This thing might have to be specified elsewhere
			PhysicalDeviceRequirementDetails requirements{};
			requirements.GraphicsSupport = VK_TRUE;
			requirements.PresentSupport = VK_TRUE;
			requirements.TransferSupport = VK_TRUE;
			// TODO: Compute support might be enabled when RT support is implemented
			requirements.ComputeSupport = VK_FALSE;
			requirements.SamplerAnisotropy = VK_TRUE;
			requirements.DiscreteGPU = VK_TRUE;
			requirements.DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			PhysicalDeviceQueueFamilyDetails queueFamilyIndices{};

			bool result = IsPhysicalDeviceSuitable(
				physicalDevice, m_ContextSurface,
				properties, features, requirements,
				queueFamilyIndices, m_SwapchainSupport
			);

			if (result) {
				CHOPPER_LOG_INFO("Picked device: {}", properties.deviceName);

				CHOPPER_LOG_INFO("GPU Driver Version: {0}.{1}.{2}",
					VK_VERSION_MAJOR(properties.driverVersion),
					VK_VERSION_MINOR(properties.driverVersion),
					VK_VERSION_PATCH(properties.driverVersion)
				);
				CHOPPER_LOG_INFO("Vulkan API Version: {0}.{1}.{2}",
					VK_VERSION_MAJOR(properties.apiVersion),
					VK_VERSION_MINOR(properties.apiVersion),
					VK_VERSION_PATCH(properties.apiVersion)
				);

				for (uint32_t j = 0; j < memory.memoryHeapCount; ++j) {
					float memorySize = ((static_cast<uint32_t>(memory.memoryHeaps[j].size)) / 1024.0f / 1024.0f / 1024.0f);
					if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
						CHOPPER_LOG_INFO("Local GPU memory: {} GiB", memorySize);
					else
						CHOPPER_LOG_INFO("Shared system memory: {} GiB", memorySize);
				}

				m_PhysicalDevice = physicalDevice;
				m_QueueFamilyIndices = queueFamilyIndices;
				m_PhysicalDeviceDetails.Properties = properties;
				m_PhysicalDeviceDetails.Features = features;
				m_PhysicalDeviceDetails.MemoryProperties = memory;

				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE) {
			CHOPPER_LOG_CRIT("No devices that satisfy requirements found!");
			return false;
		}

		CHOPPER_LOG_INFO("Physical Device picked successfully.");
		return true;
	}

	bool VulkanDevice::IsPhysicalDeviceSuitable(
		VkPhysicalDevice device, VkSurfaceKHR surface,
		const VkPhysicalDeviceProperties& properties,
		const VkPhysicalDeviceFeatures& features,
		const PhysicalDeviceRequirementDetails& requirements,
		PhysicalDeviceQueueFamilyDetails& queueFamilyIndices,
		SwapchainSupportDetails& swapchainSupport
	) {
		// Overwriting values to default ones
		queueFamilyIndices.GraphicsFamilyIndex = (uint32_t)-1;
		queueFamilyIndices.PresentFamilyIndex  = (uint32_t)-1;
		queueFamilyIndices.ComputeFamilyIndex  = (uint32_t)-1;
		queueFamilyIndices.TransferFamilyIndex = (uint32_t)-1;

		// Discrete GPU requirement
		if (requirements.DiscreteGPU && properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			CHOPPER_LOG_INFO("Candidate device is not a discrete GPU, but one is required.");
			return false;
		}

		// Selecting queue families
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		uint32_t minTransferScore = 255;
		uint32_t i = 0;
		for (const auto& queueFamily : queueFamilies) {
			uint32_t transferScore = 0;

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queueFamilyIndices.GraphicsFamilyIndex = i;
				++transferScore;
			}

			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
				queueFamilyIndices.ComputeFamilyIndex = i;
				++transferScore;
			}

			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
				if (transferScore <= minTransferScore) {
					minTransferScore = transferScore;
					queueFamilyIndices.TransferFamilyIndex = i;
				}
			}

			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_ContextSurface, &presentSupport);
			if (presentSupport)
				queueFamilyIndices.PresentFamilyIndex = i;

			++i;
		}

		// TODO: Print information about device queues

		CHECK_QUEUE_SUPPORT(requirements.GraphicsSupport, queueFamilyIndices.GraphicsFamilyIndex, "Graphics Queue");
		CHECK_QUEUE_SUPPORT(requirements.PresentSupport,  queueFamilyIndices.PresentFamilyIndex,  "Present Queue");
		CHECK_QUEUE_SUPPORT(requirements.ComputeSupport,  queueFamilyIndices.ComputeFamilyIndex,  "Compute Queue");
		CHECK_QUEUE_SUPPORT(requirements.TransferSupport, queueFamilyIndices.TransferFamilyIndex, "Transfer Queue");

		// Device must support presentation / swapchain
		swapchainSupport = QuerySwapchainSupport(device, m_ContextSurface);
		if (swapchainSupport.Formats.empty() || swapchainSupport.PresentModes.empty()) {
			CHOPPER_LOG_INFO("Candidate device does not satisfy swapchain support.");
			return false;
		}

		// Device must satisfy required extensions
		if (!requirements.DeviceExtensions.empty()) {
			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

			std::unordered_set<std::string> unsupportedExtensions(requirements.DeviceExtensions.begin(), requirements.DeviceExtensions.end());
			for (const auto& extension : availableExtensions) {
				std::string extensionName = extension.extensionName;
				if (unsupportedExtensions.count(extensionName))
					unsupportedExtensions.erase(extensionName);
			}

			if (!unsupportedExtensions.empty()) {
				// TODO: Log which extensions are not supported
				CHOPPER_LOG_INFO("Candidate device does not support all extensions.");
				return false;
			}
		}

		// Sampler anisotropy requirement
		if (requirements.SamplerAnisotropy && !features.samplerAnisotropy) {
			CHOPPER_LOG_INFO("Candidate device does not support SamplerAnisotropy.");
			return false;
		}

		return true;
	}

	SwapchainSupportDetails VulkanDevice::QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
		SwapchainSupportDetails details{};
		
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.Capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
		if (formatCount) {
			details.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
		if (presentModeCount) {
			details.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.PresentModes.data());
		}

		return details;
	}

}
