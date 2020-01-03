#include "renderer/vulkan/material/vulkan_texture_sampler.h"

namespace estun
{
	VulkanTextureSampler::VulkanTextureSampler(const uint32_t& mipLevels)
	    : VulkanTextureSampler(mipLevels, VK_FILTER_LINEAR, VK_FILTER_LINEAR,
	            VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT,
			    VK_SAMPLER_ADDRESS_MODE_REPEAT,VK_SAMPLER_ADDRESS_MODE_REPEAT,
			    VK_FALSE, VK_COMPARE_OP_ALWAYS, VK_TRUE, 16, VK_BORDER_COLOR_INT_OPAQUE_BLACK)
    {

    }

	VulkanTextureSampler::VulkanTextureSampler(const uint32_t& mipLevels, VkFilter magFilter, VkFilter minFilter,
			               VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			               VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			               VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			               VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			               VkBool32 compareEnabled = VK_FALSE, VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS,
			               VkBool32 anisotropyEnable = VK_TRUE, uint16_t maxAnisotropy = 16,
			               VkBorderColor borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = magFilter;
        samplerInfo.minFilter = minFilter;
        samplerInfo.addressModeU = addressModeU;
        samplerInfo.addressModeV = addressModeV;
        samplerInfo.addressModeW = addressModeW;
        samplerInfo.anisotropyEnable = anisotropyEnable;
        samplerInfo.maxAnisotropy = maxAnisotropy;
        samplerInfo.borderColor = borderColor;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = compareEnabled;
        samplerInfo.compareOp = compareOp;
        samplerInfo.mipmapMode = mipmapMode;
        samplerInfo.minLod = 0;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0;

        if (vkCreateSampler(*VulkanDeviceLocator::GetLogicalDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to create texture sampler");
        }
    }

	VulkanTextureSampler::~VulkanTextureSampler()
    {
        vkDestroySampler(*VulkanDeviceLocator::GetLogicalDevice(), textureSampler, nullptr);
    }

    VkSampler* GetTextureSampler()
    {
        return &textureSampler;
    }
}
