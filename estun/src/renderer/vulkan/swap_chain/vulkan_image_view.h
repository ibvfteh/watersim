#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "renderer/vulkan/swap_chain/vulkan_swap_chain.h"

namespace estun
{
    class VulkanImageView
    {
    private:
        std::vector<VkImageView> swapChainImageViews;
    public:
        VulkanImageView(VulkanSwapChain* swapChain);
        ~VulkanImageView();

        std::vector<VkImageView>* GetSwapChainImageViewsVector();
    };
}
