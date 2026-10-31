#include "Window.h"
#include "Window.h"

namespace LEV::platform {
	Window::Window(int width, int height)
		: mWidth(width), mHeight(height)
	{
		if (!glfwInit())
		{
			throw std::runtime_error("glfwInit failed!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		mWindow = glfwCreateWindow(mWidth, mHeight, "LilyEngine_V", nullptr, nullptr);
		if (!mWindow)
		{
			glfwTerminate();
			throw std::runtime_error("glfwCreateWindow failed!");
		}

		glfwSetWindowUserPointer(mWindow, this);
		glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	}

	void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{   
		auto self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		self->mWidth = width;
		self->mHeight = height;
		self->mOnResize(width, height);
	}

	Window::~Window()
	{
		if (mWindow)
			glfwDestroyWindow(mWindow);
		glfwTerminate();
	}

	bool Window::shouldClose() const
	{
		return mWindow && glfwWindowShouldClose(mWindow);
	}

	void Window::pollEvents()
	{
		glfwPollEvents();
	}

	

}
