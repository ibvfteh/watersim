#include "renderer/vulkan/swap_chain/vulkan_resources.h"
#include "renderer/vulkan/context/vulkan_device.h"

namespace estun
{
    VulkanDepthResources::VulkanDepthResources(const VkExtent2D* swapChainExtent, VkSampleCountFlagBits msaaSamples)
    {
        VkFormat depthFormat = FindDepthFormat();
        textureImage = new VulkanTextureImage(
                swapChainExtent->width, 
                swapChainExtent->height,
                1,
                msaaSamples,
                depthFormat,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        textureImageView = new VulkanTextureImageView(textureImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

    }

    VulkanDepthResources::~VulkanDepthResources()
    {
        delete textureImage; 
        delete textureImageView;
    }

    VkFormat VulkanDepthResources::FindDepthFormat() 
    {
        return FindSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    VkFormat VulkanDepthResources::FindSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        const VkFormatFeatureFlags& features) 
    {
        for (VkFormat format : candidates) 
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(*VulkanDeviceLocator::GetPhysicalDevice(), format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            } 
            
            if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
            {
                return format;
            }
        }

        ES_CORE_ASSERT("Failed to find supported format");
    }
    
    bool VulkanDepthResources::HasStencilComponent(const VkFormat& format) 
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }
	
	VkImage* VulkanDepthResources::GetDepthImage()
    {
        return textureImage->GetTextureImage();
    }
		
	VkDeviceMemory* VulkanDepthResources::GetDepthImageMemory()
    {
        return textureImage->GetTextureImageMemory();
    }
	
	VkImageView* VulkanDepthResources::GetDepthImageView()
    {
        return textureImageView->GetTextureImageView();
    }

    VulkanColorResources::VulkanColorResources(VulkanSwapChain* swapChain, VkSampleCountFlagBits msaaSamples)
    {
        VkFormat colorFormat = *swapChain->GetSwapChainImageFormat();
        textureImage = new VulkanTextureImage(
                swapChain->GetSwapChainExtent()->width, 
                swapChain->GetSwapChainExtent()->height,
                1,
                msaaSamples,
                colorFormat,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        textureImageView = new VulkanTextureImageView(textureImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

    }

    VulkanColorResources::~VulkanColorResources()
    {
        delete textureImage; 
        delete textureImageView;
    }
	
	VkImage* VulkanColorResources::GetColorImage()
    {
        return textureImage->GetTextureImage();
    }
		
	VkDeviceMemory* VulkanColorResources::GetColorImageMemory()
    {
        return textureImage->GetTextureImageMemory();
    }
	
	VkImageView* VulkanColorResources::GetColorImageView()
    {
        return textureImageView->GetTextureImageView();
    }

}
