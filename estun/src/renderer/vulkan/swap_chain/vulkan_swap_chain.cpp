#include "renderer/vulkan/swap_chain/vulkan_swap_chain.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include <algorithm>

namespace estun
{
    VulkanSwapChain::VulkanSwapChain(
            VulkanSurface* surface,
            QueueFamilyIndices indices, 
		    const uint32_t& width, 
			const uint32_t& height)
    {
        SwapChainSupportDetails swapChainSupport = VulkanDevice::QuerySwapChainSupport(*VulkanDeviceLocator::GetPhysicalDevice(), surface);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode     = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent                = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

        imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = *surface->GetSurface();

        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) 
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        } 
        else 
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform   = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = presentMode;
        createInfo.clipped        = VK_TRUE;

        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(*VulkanDeviceLocator::GetLogicalDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            ES_CORE_CRITICAL("Failed to create swap chain");
        }

        vkGetSwapchainImagesKHR(*VulkanDeviceLocator::GetLogicalDevice(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(*VulkanDeviceLocator::GetLogicalDevice(), swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
      
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        vkDestroySwapchainKHR(*VulkanDeviceLocator::GetLogicalDevice(), swapChain, nullptr);
    }

    VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        /*
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        */
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const uint32_t& width, const uint32_t& height)
    {
        if (capabilities.currentExtent.width != UINT32_MAX) 
        {
            return capabilities.currentExtent;
        }
        else 
        {
            VkExtent2D actualExtent = {width, height};

            actualExtent.width  = std::max(capabilities.minImageExtent.width,  std::min(capabilities.maxImageExtent.width,  actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkSwapchainKHR* VulkanSwapChain::GetSwapChain()
    {
        return &swapChain;
    }

    std::vector<VkImage>* VulkanSwapChain::GetSwapChainImagesVector()
    {
        return &swapChainImages;
    }

    VkFormat* VulkanSwapChain::GetSwapChainImageFormat()
    {
        return &swapChainImageFormat;
    }

    VkExtent2D* VulkanSwapChain::GetSwapChainExtent()
    {
        return &swapChainExtent;
    }

    uint32_t VulkanSwapChain::GetImageCount()
    {
        return imageCount;
    }
}
