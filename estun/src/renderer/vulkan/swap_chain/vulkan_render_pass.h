#pragma once

#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanRenderPass
    {
    private:
        VkRenderPass renderPass;
        
    public:
        VulkanRenderPass(const VkFormat& swapChainImageFormat, const VkSampleCountFlagBits msaaSamples);
        ~VulkanRenderPass();

        VkRenderPass* GetRenderPass();
    };
}
