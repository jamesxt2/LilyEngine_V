#include "SwapChain.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace LEV::vulkancore {

	SwapChain::SwapChain(const Ref<Device>& device, 
		const Ref<platform::Window>& window, 
		const Ref<WindowSurface>& surface)
		: mDevice(device), mWindow(window), mSurface(surface)
	{
		auto info = querySwapChainSupportInfo();
		auto format = chooseSurfaceFormat(info.mFormats);
		auto presentMode = choosePresentMode(info.mPresentModes);
		auto extent = chooseSwapExtent(info.mCapabilities);

		std::cout << "L6.1 chosen: format=" << format.format
			<< "  presentMode=" << presentMode
			<< "  extent=" << extent.width << "x" << extent.height << std::endl;

		mImageCount = info.mCapabilities.minImageCount + 1;
		if (info.mCapabilities.maxImageCount > 0 &&
			mImageCount > info.mCapabilities.maxImageCount)
			mImageCount = info.mCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR ci{};
		ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		ci.surface = mSurface->getSurface();
		ci.minImageCount = mImageCount;
		ci.imageColorSpace = format.colorSpace;
		ci.imageFormat = format.format;
		ci.imageExtent = extent;
		ci.imageArrayLayers = 1;
		ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		std::vector<uint32_t> indices = {
			mDevice->getGraphicQueueFamily().value(),
			mDevice->getPresentQueueFamily().value()
		};

		if (indices[0] == indices[1])
		{
			ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			ci.queueFamilyIndexCount = 0;
			ci.pQueueFamilyIndices = nullptr;
		}
		else
		{
			ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			ci.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
			ci.pQueueFamilyIndices = indices.data();
		}

		ci.preTransform = info.mCapabilities.currentTransform;
		ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		ci.presentMode = presentMode;
		ci.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(mDevice->getDevice(), &ci, nullptr, &mSwapChain) != VK_SUCCESS)
			throw std::runtime_error("Error: vkCreateSwapchainKHR failed!");

		mSwapChainFormat = format.format;
		mSwapChainExtent = extent;

		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
		mSwapChainImages.resize(mImageCount);
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChainImages.data());

		std::cout << "SwapChain created: " << mSwapChainExtent.width <<
			"x" << mSwapChainExtent.height << " images = " <<
			mImageCount << std::endl;

		mSwapChainImageViews.resize(mImageCount);
		for (uint32_t i = 0; i < mImageCount; ++i) {
			mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	SwapChain::~SwapChain()
	{
		for (auto fb : mSwapChainFramebuffers)
			vkDestroyFramebuffer(mDevice->getDevice(), fb, nullptr);

		for (auto iv : mSwapChainImageViews)
			vkDestroyImageView(mDevice->getDevice(), iv, nullptr);

		if (mSwapChain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
	}

	SwapChainSupportInfo SwapChain::querySwapChainSupportInfo()
	{
		SwapChainSupportInfo info;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);

		uint32_t count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, nullptr);
		if (count) {
			info.mFormats.resize(count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, info.mFormats.data());
		}

		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, nullptr);
		if (count) {
			info.mPresentModes.resize(count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &count, info.mPresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		for (const auto& f : formats)
			if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return f;
		return formats[0];
	}

	VkPresentModeKHR SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& modes)
	{
		for (const auto& m : modes)
			if (m == VK_PRESENT_MODE_MAILBOX_KHR)
				return m;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		int w = 0, h = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &w, &h);

		VkExtent2D extent = { static_cast<uint32_t>(w), static_cast<uint32_t>(h) };
		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return extent;
	}

	VkImageView SwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		VkImageViewCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ci.image = image;
		ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ci.format = format;

		ci.subresourceRange.aspectMask = aspectFlags;
		ci.subresourceRange.baseMipLevel = 0;
		ci.subresourceRange.levelCount = mipLevels;
		ci.subresourceRange.baseArrayLayer = 0;
		ci.subresourceRange.layerCount = 1;

		VkImageView view{ VK_NULL_HANDLE };
		if (vkCreateImageView(mDevice->getDevice(), &ci, nullptr, &view) != VK_SUCCESS)
			throw std::runtime_error("Error: Failed to create image view!");

		return view;
	}
}