#pragma once
#include "vk_common.h"
#include "Device.h"
#include "../platform/Window.h"
#include "WindowSurface.h"
namespace LEV::vulkancore {
	class RenderPass;
}

namespace LEV::vulkancore {

	struct SwapChainSupportInfo 
	{
		VkSurfaceCapabilitiesKHR mCapabilities{};
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR> mPresentModes;
	};

	class SwapChain
	{
	public:
		SwapChain(const Ref<Device>& device,
			const Ref<platform::Window>& window,
			const Ref<WindowSurface>& surface);
		~SwapChain();

		void                 createFrameBuffers(const Ref<RenderPass>& renderPass);

        SwapChainSupportInfo querySwapChainSupportInfo();
        VkSurfaceFormatKHR   chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        VkPresentModeKHR     choosePresentMode(const std::vector<VkPresentModeKHR>& modes);
        VkExtent2D           chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        [[nodiscard]] VkFormat                      getSwapChainImageFormat()    const { return mSwapChainFormat; }
        [[nodiscard]] VkExtent2D                    getSwapChainExtent()         const { return mSwapChainExtent; }
        [[nodiscard]] VkSwapchainKHR                getSwapChain()               const { return mSwapChain; }
        [[nodiscard]] uint32_t                      getImageCount()              const { return mImageCount; }
        [[nodiscard]] std::vector<VkImage>          getSwapChainImages()         const { return mSwapChainImages; }
        [[nodiscard]] std::vector<VkImageView>      getSwapChainImageViews()     const { return mSwapChainImageViews; }
        [[nodiscard]] std::vector<VkFramebuffer>    getSwapChainFramebuffers()   const { return mSwapChainFramebuffers; }

        uint32_t acquireNextImage(VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE) {
            uint32_t imageIndex = 0;
            vkAcquireNextImageKHR(mDevice->getDevice(), mSwapChain,
                std::numeric_limits<uint64_t>::max(),
                semaphore, fence, &imageIndex);
            return imageIndex;
        }

    private:
        VkImageView createImageView(VkImage image, VkFormat format,
            VkImageAspectFlags aspectFlags,
            uint32_t mipLevels = 1);

        Ref<Device>           mDevice{ nullptr };
        Ref<platform::Window> mWindow{ nullptr };
        Ref<WindowSurface>    mSurface{ nullptr };


        VkSwapchainKHR mSwapChain{ VK_NULL_HANDLE };
        VkFormat       mSwapChainFormat{};
        VkExtent2D     mSwapChainExtent{};
        uint32_t       mImageCount{ 0 };

        std::vector<VkImage>       mSwapChainImages{};
        std::vector<VkImageView>   mSwapChainImageViews{};
        std::vector<VkFramebuffer> mSwapChainFramebuffers{};
	};
}