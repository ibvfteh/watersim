#include "renderer/vulkan/texture/vulkan_texture_image.h"
#include "renderer/vulkan/vulkan_memory_manager.h"
#include "renderer/vulkan/context/vulkan_device.h"

namespace estun
{
    VulkanTextureImage::VulkanTextureImage(
            const uint32_t width, const uint32_t height, const uint32_t mipLevels,
            VkSampleCountFlagBits numSamples, VkFormat& format, VkImageTiling tiling,
            VkImageUsageFlags& usage, VkMemoryPropertyFlags& properties);
    {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(*VulkanDeviceLocator::GetLogicalDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to create image");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(*VulkanDeviceLocator::GetLogicalDevice(), textureImage, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(device, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(*VulkanDeviceLocator::GetLogicalDevice(), &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to allocate image memory");
        }

        vkBindImageMemory(*VulkanDeviceLocator::GetLogicalDevice(), textureImage, textureImageMemory, 0);
    }

    VulkanTextureImage::~VulkanTextureImage()
    {
        vkDestroyImage(*VulkanDeviceLocator::GetLogicalDevice(), textureImage, nullptr);
        vkFreeMemory(*VulkanDeviceLocator::GetLogicalDevice(), textureImageMemory, nullptr);
    }

	VkImage* GetTextureImage()
    {
        return &textureImage;
    }

	VkDeviceMemory* GetTextureImageMemory()
    {
        return &textureImageMemory;
    }
}
