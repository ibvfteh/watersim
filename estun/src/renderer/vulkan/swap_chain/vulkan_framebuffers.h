#pragma once

#include "renderer/vulkan/swap_chain/vulkan_image_view.h"
#include "renderer/vulkan/swap_chain/vulkan_render_pass.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace estun
{
    class VulkanFramebuffers
    {
    private:
        std::vector<VkFramebuffer> swapChainFramebuffers;
    public:
        VulkanFramebuffers(
                VulkanImageView* imageView,
                VulkanRenderPass* renderPass,
                VkExtent2D* swapChainExtent,
                VkImageView* depthImageView,
                VkImageView* colorImageView
                );
        ~VulkanFramebuffers();

        std::vector<VkFramebuffer>* GetSwapChainFramebuffersVector();
        size_t GetSwapChainFramebuffersVectorSize();
    };
}
