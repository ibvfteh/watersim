#include "Renderer/Vulkan/Context/Framebuffer.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Context/RenderPass.h"
#include "Renderer/Vulkan/Context/SwapChain.h"

Estun::Renderer::Vulkan::Framebuffer::Framebuffer(
    const std::vector<VkImageView> &attachments,
    const SwapChain &swapChain,
    const RenderPass &renderPass)
{
    if (attachments.size() == 0)
    {
        ES_CORE_ASSERT("Failedto create framebuffer, attachments are empty")
    }
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass.GetRenderPass();
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapChain.GetExtent().width;
    framebufferInfo.height = swapChain.GetExtent().height;
    framebufferInfo.layers = 1;

    VK_CHECK_RESULT(vkCreateFramebuffer(DeviceLocator::GetLogicalDevice(), &framebufferInfo, nullptr, &framebuffer), "Failed to create framebuffer");
}

Estun::Renderer::Vulkan::Framebuffer::Framebuffer(Framebuffer &&other) noexcept : framebuffer(other.framebuffer)
{
    other.framebuffer = nullptr;
}

Estun::Renderer::Vulkan::Framebuffer::~Framebuffer()
{
    if (framebuffer != nullptr)
    {
        vkDestroyFramebuffer(DeviceLocator::GetLogicalDevice(), framebuffer, nullptr);
        framebuffer = nullptr;
    }
}

VkFramebuffer Estun::Renderer::Vulkan::Framebuffer::GetFramebuffer() const
{
    return framebuffer;
}

/*
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
*/