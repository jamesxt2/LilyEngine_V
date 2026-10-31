#pragma once
#include "vk_common.h"
#include "Instance.h"
#include "../platform/Window.h"

namespace LEV::vulkancore {

	class WindowSurface
	{
	public:
		WindowSurface(Ref<Instance> instance, Ref<platform::Window> window);
		~WindowSurface();

		VkSurfaceKHR getSurface() const { return mSurface; }
		VkInstance getInstance() const { return mInstance->getInstance(); }

	private:
		VkSurfaceKHR mSurface{ VK_NULL_HANDLE };
		Ref<Instance> mInstance{ nullptr };
	};
}