#pragma once
#include "vk_common.h"
#include "instance.h"
#include "windowSurface.h"

namespace LEV::vulkancore {

	class Device
	{
	public:
		Device(Ref<Instance> instance, Ref<WindowSurface> surface);
		~Device();

		[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }

		[[nodiscard]] VkDevice getDevice() const { return mDevice; }
		[[nodiscard]] VkQueue getGraphicQueue() const { return mGraphicQueue; }
		[[nodiscard]] VkQueue getPresentQueue() const { return mPresentQueue; }
		[[nodiscard]] std::optional<uint32_t> getGraphicQueueFamily() const { return mGraphicQueueFamily; }
		[[nodiscard]] std::optional<uint32_t> getPresentQueueFamily() const { return mPresentQueueFamily; }

	private:

		int rateDeviceSuitability(VkPhysicalDevice device);
		void initQueueFamilies(VkPhysicalDevice device);

		void pickPhysicalDevice();
		bool isQueueFamilyComplete();

		void createLogicalDevice();

		VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
		VkDevice mDevice{ VK_NULL_HANDLE };

		Ref<Instance>      mInstance{ nullptr };
		Ref<WindowSurface> mSurface{ nullptr };

		std::optional<uint32_t> mGraphicQueueFamily;
		std::optional<uint32_t> mPresentQueueFamily;
		VkQueue mGraphicQueue{ VK_NULL_HANDLE };
		VkQueue mPresentQueue{ VK_NULL_HANDLE };
	
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	};
}