#include "renderer/vulkan/texture/vulkan_texture_image_view.h"
#include "renderer/vulkan/context/vulkan_device.h"

namespace estun
{
    VulkanTextureImage::VulkanTextureImage(VkFormat& format, VkImageAspectFlags aspectFlags, const uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = textureImage;
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

    VulkanTextureImage::~VulkanTextureImage()
    {
        vkDestroyImageView(*VulkanDeviceLocator::GetLogicalDevice(), textureImageView, nullptr);
    }

    VkImageView* VulkanTextureImage::GetTextureImageView()
    {
        return &textureImageView;
    }
}
