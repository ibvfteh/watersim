#pragma once

#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanTextureSampler
    {
    private:
	    VkSampler textureSampler;	
	public:

		VulkanTextureSampler(const uint32_t& mipLevels);
		VulkanTextureSampler(const uint32_t& mipLevels, VkFilter magFilter, VkFilter minFilter,
			               VkSamplerMipmapMode minimapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			               VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			               VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			               VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			               VkBool32 compareEnabled = VK_FALSE, VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS,
			               VkBool32 anisotropyEnable = VK_TRUE, uint16_t maxAnisotropy = 16,
			               VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK)

		~VulkanTextureSampler();

	    VkSampler* GetTextureSampler();	
	};
}
