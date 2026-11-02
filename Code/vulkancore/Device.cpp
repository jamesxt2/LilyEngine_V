#include "Device.h"
#include "Device.h"

namespace LEV::vulkancore {

	Device::Device(Ref<Instance> instance, Ref<WindowSurface> surface)
		: mInstance(instance), mSurface(surface)
	{
		pickPhysicalDevice();
		createLogicalDevice();
	}

	Device::~Device()
	{
		vkDestroyDevice(mDevice, nullptr);
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
		int memorySize = 0;
		for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
		{
			if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				score += static_cast<int>(memProps.memoryHeaps[i].size
					/ (1024 * 1024 * 1024)) * 100;
			memorySize += static_cast<int>(memProps.memoryHeaps[i].size
				/ (1024 * 1024 * 1024));
		}
		std::cout << props.deviceName << " size: " << memorySize << "GB" << std::endl;
		return score;
	}

	void Device::initQueueFamilies(VkPhysicalDevice device)
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
				initQueueFamilies(device);
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

	void Device::createLogicalDevice()
	{
		// get queue family indices
		std::set<uint32_t> uniqueFamilies = {
			mGraphicQueueFamily.value(),
			mPresentQueueFamily.value()
		};

		float priority = 1.0f;
		std::vector<VkDeviceQueueCreateInfo> queueCIs;
		for (uint32_t family : uniqueFamilies)
		{
			VkDeviceQueueCreateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			ci.queueFamilyIndex = family;
			ci.queueCount = 1;
			ci.pQueuePriorities = &priority;
			queueCIs.push_back(ci);
		}

		// feature only what we need in part1
		VkPhysicalDeviceFeatures features{};
		features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		ci.queueCreateInfoCount = static_cast<uint32_t>(queueCIs.size());
		ci.pQueueCreateInfos = queueCIs.data();
		ci.pEnabledFeatures = &features;
		ci.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		ci.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(mPhysicalDevice, &ci, nullptr, &mDevice) != VK_SUCCESS)
			throw std::runtime_error("Error: failed to create logical device!");

		vkGetDeviceQueue(mDevice, mGraphicQueueFamily.value(), 0, &mGraphicQueue);
		vkGetDeviceQueue(mDevice, mPresentQueueFamily.value(), 0, &mPresentQueue);
	}
}