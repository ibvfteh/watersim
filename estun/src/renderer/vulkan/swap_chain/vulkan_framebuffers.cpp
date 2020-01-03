#include "renderer/vulkan/swap_chain/vulkan_framebuffers.h"
#include "renderer/vulkan/context/vulkan_device.h"
#include "core/core.h"

namespace estun
{
    VulkanFramebuffers::VulkanFramebuffers(
            VulkanImageView* imageView,
            VulkanRenderPass* renderPass,
            VkExtent2D* swapChainExtent,
            VkImageView* depthImageView,
            VkImageView* colorImageView
            )
    {
        const std::vector<VkImageView>* swapChainImageViews = imageView->GetSwapChainImageViewsVector();

        swapChainFramebuffers.resize(swapChainImageViews->size());

        for (size_t i = 0; i < swapChainImageViews->size(); i++) {
            std::array<VkImageView, 3> attachments = {
                *colorImageView,
                *depthImageView,
                (*swapChainImageViews)[i]
            };

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = *renderPass->GetRenderPass();
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent->width;
            framebufferInfo.height = swapChainExtent->height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(*VulkanDeviceLocator::GetLogicalDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) 
            {
                ES_CORE_ASSERT("Failed to create framebuffer");
            }
        }
    }
    
    VulkanFramebuffers::~VulkanFramebuffers()
    {
        for (auto const framebuffer : swapChainFramebuffers)
        {
            vkDestroyFramebuffer(*VulkanDeviceLocator::GetLogicalDevice(), framebuffer, nullptr);
        }
    }

    std::vector<VkFramebuffer>* VulkanFramebuffers::GetSwapChainFramebuffersVector() 
    {
        return &swapChainFramebuffers;
    }

    size_t VulkanFramebuffers::GetSwapChainFramebuffersVectorSize() 
    {
        return swapChainFramebuffers.size();
    }
}
