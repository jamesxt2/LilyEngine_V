#pragma once
#include "../Common.h"

namespace LEV::platform {
	class Window
	{
	public:
		Window(int width, int height);
		~Window();

		bool shouldClose() const;
		void pollEvents();

		using ResizeFn = std::function<void(int, int)>;
		void setOnResize(ResizeFn fn) { mOnResize = std::move(fn); }
		GLFWwindow* getWindow() { return mWindow; }
	private:
		ResizeFn mOnResize;
		GLFWwindow* mWindow;
		int mWidth, mHeight;
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	};
}