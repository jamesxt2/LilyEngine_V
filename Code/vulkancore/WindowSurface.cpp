#include "WindowSurface.h"
#include <stdexcept>

namespace LEV::vulkancore {

	WindowSurface::WindowSurface(Ref<Instance> instance, Ref<platform::Window> window)
		: mInstance(instance)
	{
		if (glfwCreateWindowSurface(instance->getInstance(),
			window->getWindow(), nullptr, &mSurface) != VK_SUCCESS)
			throw std::runtime_error("Error: failed to create window surface!");
	}

	WindowSurface::~WindowSurface()
	{
		if (mSurface != VK_NULL_HANDLE)
			vkDestroySurfaceKHR(mInstance->getInstance(), mSurface, nullptr);
	}
}