#pragma once

#include "renderer/vulkan/material/vulkan_texture_image.h"
#include "renderer/vulkan/material/vulkan_texture_image_view.h"
#include "renderer/vulkan/swap_chain/vulkan_swap_chain.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace estun
{
    class VulkanDepthResources
    {
    private:
		VulkanTextureImage* textureImage;
		VulkanTextureImageView* textureImageView;

	public:
		VulkanDepthResources(const VkExtent2D* swapChainExtent, VkSampleCountFlagBits msaaSamples);
		~VulkanDepthResources();

        static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, const VkFormatFeatureFlags& features);
		static VkFormat FindDepthFormat();
		static bool     HasStencilComponent(const VkFormat& format);

		VkImage*        GetDepthImage();
		VkDeviceMemory* GetDepthImageMemory();
		VkImageView*    GetDepthImageView();
    };

    class VulkanColorResources
    {
    private:
		VulkanTextureImage* textureImage;
		VulkanTextureImageView* textureImageView;

	public:
		VulkanColorResources(VulkanSwapChain* swapChain, VkSampleCountFlagBits msaaSamples);
		~VulkanColorResources();

		VkImage*        GetColorImage();
		VkDeviceMemory* GetColorImageMemory();
		VkImageView*    GetColorImageView();
    }; 
}
