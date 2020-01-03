#include "renderer/vulkan/swap_chain/vulkan_image_view.h"
#include "renderer/vulkan/context/vulkan_device.h"

namespace estun
{
    VulkanImageView::VulkanImageView(VulkanSwapChain* swapChain)
    {
        const std::vector<VkImage>* swapChainImages = swapChain->GetSwapChainImagesVector();
        swapChainImageViews.resize(swapChainImages->size());

        for (size_t i = 0; i < swapChainImages->size(); i++) 
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = (*swapChainImages)[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = *swapChain->GetSwapChainImageFormat();

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;


            if (vkCreateImageView(*VulkanDeviceLocator::GetLogicalDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
            {
                ES_CORE_ASSERT("Failed to create image views");
            }       
        }
    }
    
    VulkanImageView::~VulkanImageView()
    {
        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(*VulkanDeviceLocator::GetLogicalDevice(), imageView, nullptr);
        }
    }

    std::vector<VkImageView>* VulkanImageView::GetSwapChainImageViewsVector()
    {
        return &swapChainImageViews;
    }
}
