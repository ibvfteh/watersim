#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace estun
{
    class VulkanTextureImage
    {
    private:
		VkImage        textureImage;
		VkDeviceMemory textureImageMemory;
		uint32_t       mipLevels;
	public:
    	VulkanTextureImage(std::string filename);
		VulkanTextureImage(
		        const uint32_t width, const uint32_t height, const uint32_t mipLevels,
                VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
                VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		~VulkanTextureImage();

		void CreateImage(
		        const uint32_t width, const uint32_t height, const uint32_t mipLevels,
                VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
                VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

    	void TransitionImageLayout(VkImage* image, const VkFormat format,
                const VkImageLayout &oldLayout, const VkImageLayout &newLayout);

		void GenerateMipmaps(VkImage *image, VkFormat imageFormat,
				const int32_t texWidth, const int32_t texHeight, const uint32_t mipLevels);

		VkImage*        GetTextureImage();
		VkDeviceMemory* GetTextureImageMemory();
		uint32_t        GetMipLevels();
	};
}
