#include "VulkanApp.h"

namespace LEV {
	void VulkanApp::run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanUp();
	}

	void VulkanApp::initWindow()
	{
		mWindow = MakeRef<platform::Window>(mWidth, mHeight);
		mWindow->setOnResize(
			[this](int w, int h) {
				this->onResize(w, h);
			}
		);
	}

	void VulkanApp::initVulkan()
	{
		mInstance = MakeRef<vulkancore::Instance>(true);
		mSurface = MakeRef<vulkancore::WindowSurface>(mInstance, mWindow);
		auto testDevice = MakeRef<vulkancore::Device>(mInstance, mSurface);
	}

	void VulkanApp::mainLoop()
	{
		while (!mWindow->shouldClose())
		{
			mWindow->pollEvents();
		}
	}

	void VulkanApp::cleanUp()
	{
		mSurface.reset();
		mInstance.reset();
		mWindow.reset();
	}

	void VulkanApp::onResize(int w, int h)
	{
		mWidth = w;
		mHeight = h;
		mFrambufferResized = true;
	}

}