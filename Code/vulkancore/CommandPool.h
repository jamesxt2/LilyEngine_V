#pragma once
#include "vk_common.h"
#include "Device.h"

namespace LEV::vulkancore {

	class CommandPool
	{
	public:
		CommandPool(const Ref<Device>& device,
			VkCommandPoolCreateFlagBits flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		~CommandPool();

		[[nodiscard]] VkCommandPool getCommandPool() const { return mCommandPool; }

	private:
		VkCommandPool mCommandPool{ VK_NULL_HANDLE };
		Ref<Device> mDevice{ nullptr };
	};
}