#pragma once

#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanTextureImage
    {
    protected:
		VkImage        textureImage;
		VkDeviceMemory textureImageMemory;
	public:

		VulkanTextureImage(
		        const uint32_t width, const uint32_t height, const uint32_t mipLevels,
                VkSampleCountFlagBits numSamples, VkFormat& format, VkImageTiling tiling,
                VkImageUsageFlags& usage, VkMemoryPropertyFlags& properties);

		~VulkanTextureImage();

		VkImage*        GetTextureImage();
		VkDeviceMemory* GetTextureImageMemory();
	};
}
