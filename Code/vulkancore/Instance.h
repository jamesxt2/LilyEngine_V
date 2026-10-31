#pragma once
#include "vk_common.h"

namespace LEV::vulkancore {

	class Instance
	{
	public:
		explicit Instance(bool enableValidationLayer = true);
		~Instance();

		VkInstance getInstance() const { return mInstance; }

	private:
		void setupDebugger();
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();
		void printAvailableExtensions();

		VkInstance mInstance{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT mDebugger{ VK_NULL_HANDLE };
		bool mEnableValidationLayer{ true };

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	};
}