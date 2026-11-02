#include "CommandPool.h"

namespace LEV::vulkancore {

	CommandPool::CommandPool(const Ref<Device>& device, VkCommandPoolCreateFlagBits flags)
		: mDevice(device)
	{
		VkCommandPoolCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		ci.queueFamilyIndex = mDevice->getGraphicQueueFamily().value();
		ci.flags = flags;

		if (vkCreateCommandPool(mDevice->getDevice(), &ci, nullptr, &mCommandPool) != VK_SUCCESS)
			throw std::runtime_error("Error: Failed to create command pool!");
	}

	CommandPool::~CommandPool()
	{
		if (mCommandPool != VK_NULL_HANDLE)
			vkDestroyCommandPool(mDevice->getDevice(), mCommandPool, nullptr);
	}

}

