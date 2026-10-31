#include "Device.h"

namespace LEV::vulkancore {

	Device::Device(Ref<Instance> instance, Ref<WindowSurface> surface)
		: mInstance(instance), mSurface(surface)
	{
		pickPhysicalDevice();
	}

	Device::~Device()
	{
	}

	int Device::rateDeviceSuitability(VkPhysicalDevice device)
	{
		int score = 0;
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device, &props);

		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
		std::vector<VkExtensionProperties> exts(extCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, exts.data());
		bool hasSwapChain = false;
		for (const auto& ext : exts)
		{
			if (std::string(ext.extensionName) == VK_KHR_SWAPCHAIN_EXTENSION_NAME)
			{
				hasSwapChain = true;
				break;
			}
		}
		if (!hasSwapChain) return 0;

		uint32_t qfCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qfCount, nullptr);
		std::vector<VkQueueFamilyProperties> qfs(qfCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qfCount, qfs.data());

		VkBool32 hasGraphic = false, hasPresent = false;
		for (uint32_t i = 0; i < qfCount; ++i)
		{
			if (qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				hasGraphic = true;
			if (!hasPresent)
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->getSurface(), &hasPresent);
			if (hasGraphic && hasPresent) break;
		}

		if (!hasGraphic || !hasPresent) return 0;

		switch (props.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score += 10000;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score += 5000;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			score += 1000;
			break;
		default: break;
		}

		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(device, &memProps);
		for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
		{
			if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				score += static_cast<int>(memProps.memoryHeaps[i].size
					/ (1024 * 1024 * 1024)) * 100;
		}
		return score;
	}

	void Device::queryQueueFamilies(VkPhysicalDevice device)
	{
		mGraphicQueueFamily = std::nullopt;
		mPresentQueueFamily = std::nullopt;

		uint32_t qfCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qfCount, nullptr);
		std::vector<VkQueueFamilyProperties> qfs(qfCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qfCount, qfs.data());

		for (uint32_t i = 0; i < qfCount; ++i)
		{
			if (qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				mGraphicQueueFamily = i;
			VkBool32 hasPresent = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->getSurface(), &hasPresent);
			if (hasPresent)
				mPresentQueueFamily = i;
			if (isQueueFamilyComplete()) break;
		}
	}

	void Device::pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("Error: No vulkan-capable GPU found!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance->getInstance(), &deviceCount, devices.data());

		int highestScore = 0;
		VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

		for (const auto& device : devices)
		{
			int score = rateDeviceSuitability(device);

			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(device, &props);
			std::cout << "Found GPU: " << props.deviceName << "|Score: " << score << std::endl;

			if (score > highestScore)
			{
				highestScore = score;
				bestDevice = device;
				queryQueueFamilies(device);
			}
		}

		if (bestDevice == VK_NULL_HANDLE || highestScore == 0)
			throw std::runtime_error("Error: No suitable GPU found!");

		mPhysicalDevice = bestDevice;

		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(mPhysicalDevice, &props);
		std::cout << "Selected GPU: " << props.deviceName << "(Score: " << highestScore << ")" << std::endl;
	}

	bool Device::isQueueFamilyComplete()
	{
		return mGraphicQueueFamily.has_value() && mPresentQueueFamily.has_value();
	}
}