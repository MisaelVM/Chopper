#pragma once

#include <vulkan/vulkan.hpp>

namespace Chopper {

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	struct PhysicalDeviceQueueFamilyDetails {
		uint32_t GraphicsFamilyIndex = (uint32_t)-1;
		uint32_t PresentFamilyIndex = (uint32_t)-1;
		uint32_t ComputeFamilyIndex = (uint32_t)-1;
		uint32_t TransferFamilyIndex = (uint32_t)-1;
	};

	struct PhysicalDeviceRequirementDetails {
		VkBool32 GraphicsSupport;
		VkBool32 PresentSupport;
		VkBool32 ComputeSupport;
		VkBool32 TransferSupport;
		VkBool32 SamplerAnisotropy;
		VkBool32 DiscreteGPU;
		std::vector<const char*> DeviceExtensions;
	};

	struct VulkanDevice {
		friend class VulkanContext;
	public:
		VulkanDevice(VulkanContext& context);
		~VulkanDevice();

		VkPhysicalDevice& Physical() { return m_PhysicalDevice; }
		VkDevice& Logical() { return m_LogicalDevice; }

		const PhysicalDeviceQueueFamilyDetails& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		const SwapchainSupportDetails& GetSwapchainSupportDetails() const { return m_SwapchainSupport; }
		SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, bool update = false);
		bool FindDepthFormat();

	private:
		bool CreateDevice();
		void DestroyDevice();

		bool PickPhysicalDevice();
		bool IsPhysicalDeviceSuitable(
			VkPhysicalDevice device, VkSurfaceKHR surface,
			const VkPhysicalDeviceProperties& properties,
			const VkPhysicalDeviceFeatures& features,
			const PhysicalDeviceRequirementDetails& requirements,
			PhysicalDeviceQueueFamilyDetails& queueFamilyIndices,
			SwapchainSupportDetails& swapchainSupport
		);

		PhysicalDeviceQueueFamilyDetails m_QueueFamilyIndices{};
		SwapchainSupportDetails m_SwapchainSupport{};

		struct PhysicalDeviceDetails {
			VkPhysicalDeviceProperties Properties;
			VkPhysicalDeviceFeatures Features;
			VkPhysicalDeviceMemoryProperties MemoryProperties;
		} m_PhysicalDeviceDetails{};

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;
		VkQueue m_TransferQueue = VK_NULL_HANDLE;

		VkInstance& m_ContextInstance;
		VkSurfaceKHR& m_ContextSurface;
		VkAllocationCallbacks*& m_ContextAllocators;
	};

}