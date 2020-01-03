#pragma once

#include <vulkan/vulkan.h>

#include "renderer/vulkan/material/vulkan_texture_image.h"

namespace estun
{
    class VulkanTextureImageView
    {
    private:
		VkImageView    textureImageView;
	public:
		VulkanTextureImageView(VkImage* image, const uint32_t mipLevels);

		VulkanTextureImageView(VulkanTextureImage* image, VkFormat& format, VkImageAspectFlags aspectFlags, const uint32_t mipLevels);

		~VulkanTextureImageView();

	    VkImageView* GetTextureImageView();	
	};
}
