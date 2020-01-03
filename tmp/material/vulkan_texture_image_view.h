#pragma once

#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanTextureImageView
    {
    private:
		VkImageView    textureImageView;
	public:

		VulkanTextureImageView(VkFormat& format, VkImageAspectFlags aspectFlags, const uint32_t mipLevels);

		~VulkanTextureImageView();

	    VkImageView* GetTextureImageView();	
	};
}
