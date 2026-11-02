#pragma once
#include "Common.h"
#include "platform/Window.h"
#include "vulkancore/Instance.h"
#include "vulkancore/WindowSurface.h"
#include "vulkancore/Device.h"
#include "vulkancore/CommandPool.h"
#include "vulkancore/SwapChain.h"

namespace LEV {
	class VulkanApp {
	public:
		void run();

	private:
		void initWindow();
		void initVulkan();
		void mainLoop();
		void cleanUp();
		void onResize(int w, int h);

		Ref<platform::Window> mWindow{ nullptr };
		Ref<vulkancore::Instance> mInstance{ nullptr };
		Ref<vulkancore::WindowSurface> mSurface{ nullptr };
		Ref<vulkancore::Device> mDevice{ nullptr };
		Ref<vulkancore::CommandPool> mCommandPool{ nullptr };
		Ref<vulkancore::SwapChain> mSwapChain{ nullptr };
		int mWidth{ 1280 }, mHeight{ 720 };
		bool mFrambufferResized{ false };
	};
}