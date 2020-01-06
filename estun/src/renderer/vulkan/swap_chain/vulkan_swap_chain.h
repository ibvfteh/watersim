#pragma once

#include "renderer/vulkan/context/vulkan_surface.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace estun
{
    class VulkanSwapChain
    {
    public:
    private:
        VkSwapchainKHR                 swapChain;
        std::vector<VkImage>           swapChainImages;
        VkFormat                       swapChainImageFormat;
        VkExtent2D                     swapChainExtent;
        uint32_t                       imageCount = -1;
    public:
        VulkanSwapChain(
                        VulkanSurface* surface,
                        QueueFamilyIndices indices, 
		                const uint32_t& width, 
		                const uint32_t& height);
        ~VulkanSwapChain();

        VkSwapchainKHR* GetSwapChain();

        std::vector<VkImage>* GetSwapChainImagesVector();
        VkFormat*             GetSwapChainImageFormat();
        VkExtent2D*           GetSwapChainExtent();
        uint32_t              GetImageCount();

    private:
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const uint32_t& width, const uint32_t& height);
    };
}
