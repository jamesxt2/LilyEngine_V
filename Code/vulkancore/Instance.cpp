#include "Instance.h"
#include "Instance.h"

namespace LEV::vulkancore {

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pMessageData,
		void* pUserData)
	{
		std::cout << "ValidationLayer: " << pMessageData->pMessage << std::endl;
		return VK_FALSE;
	}

	static VkResult CreateDebugUtilsMessengerExt(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* debugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, debugMessenger);
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}

	Instance::Instance(bool enableValidationLayer)
		: mEnableValidationLayer(enableValidationLayer)
	{
		if (mEnableValidationLayer && !checkValidationLayerSupport())
		{
			throw std::runtime_error("Error: Validation layer is not supported!");
		}

		uint32_t supportedVersion = VK_API_VERSION_1_0;
		vkEnumerateInstanceVersion(&supportedVersion);
		if (supportedVersion < VK_API_VERSION_1_4)
		{
			throw std::runtime_error("Error: Vulkan 1.4 not supported on this system");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "LilyEngine_V";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "NO ENGINE";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_4;

		auto extensions = getRequiredExtensions();
		VkInstanceCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		ci.pApplicationInfo = &appInfo;
		ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		ci.ppEnabledExtensionNames = extensions.data();
		if (mEnableValidationLayer)
		{
			ci.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			ci.ppEnabledLayerNames = validationLayers.data();
		}

		if (vkCreateInstance(&ci, nullptr, &mInstance) != VK_SUCCESS)
		{
			throw std::runtime_error("Error: failed to create instance");
		}
		setupDebugger();
	}

	Instance::~Instance()
	{
		if (mEnableValidationLayer)
			DestroyDebugUtilsMessengerEXT(mInstance, mDebugger, nullptr);
		vkDestroyInstance(mInstance, nullptr);
	}

	void Instance::setupDebugger()
	{
		if (!mEnableValidationLayer) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pfnUserCallback = debugCallback;

		if (CreateDebugUtilsMessengerExt(mInstance, &createInfo, nullptr, &mDebugger) != VK_SUCCESS)
			throw std::runtime_error("Error: failed to create debugger!");
	}

	bool Instance::checkValidationLayerSupport()
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto& layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto& layerProp : availableLayers)
			{
				if (std::strcmp(layerProp.layerName, layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound) return false;
		}
		return true;
	}

	std::vector<const char*> Instance::getRequiredExtensions()
	{
		uint32_t glfwExtCount = 0;
		const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		std::vector<const char*> extensions(glfwExts, glfwExts + glfwExtCount);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		return extensions;
	}

	void Instance::printAvailableExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "Available extensions:" << std::endl;
		for (const auto& ext : extensions)
		{
			std::cout << " " << ext.extensionName << std::endl;
		}
	}
}