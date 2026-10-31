#pragma once
#include "Common.h"
#include "platform/Window.h"
#include "vulkancore/Instance.h"
#include "vulkancore/WindowSurface.h"
#include "vulkancore/Device.h"

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

		Ref<platform::Window> mWindow;
		Ref<vulkancore::Instance> mInstance;
		Ref<vulkancore::WindowSurface> mSurface;
		int mWidth{ 1280 }, mHeight{ 720 };
		bool mFrambufferResized{ false };
	};
}