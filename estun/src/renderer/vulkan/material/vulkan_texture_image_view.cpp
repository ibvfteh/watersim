#include "renderer/vulkan/material/vulkan_texture_image_view.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include "core/core.h"

namespace estun
{
    VulkanTextureImageView::VulkanTextureImageView(VkImage* image, const uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = *image;
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = VK_FORMAT_R8G8B8A8_UNORM;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        VkImageView imageView;
        if (vkCreateImageView(*VulkanDeviceLocator::GetLogicalDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to create texture image view");
        }
    }

    VulkanTextureImageView::VulkanTextureImageView(VulkanTextureImage* image, VkFormat& format, VkImageAspectFlags aspectFlags, const uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = *image->GetTextureImage();
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = format;
        viewInfo.subresourceRange.aspectMask     = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        VkImageView imageView;
        if (vkCreateImageView(*VulkanDeviceLocator::GetLogicalDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to create texture image view");
        }
    }

    VulkanTextureImageView::~VulkanTextureImageView()
    {
        vkDestroyImageView(*VulkanDeviceLocator::GetLogicalDevice(), textureImageView, nullptr);
    }

    VkImageView* VulkanTextureImageView::GetTextureImageView()
    {
        return &textureImageView;
    }
}
