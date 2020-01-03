#include "renderer/vulkan/material/vulkan_descriptor_pool.h"
#include "renderer/vulkan/vulkan_context.h"

namespace estun
{
    VulkanDescriptorPool::VulkanDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes = {};
        uint32_t swapChainImagesSize = VulkanContextLocator::GetContext()->GetImageView()->GetSwapChainImageViewsVector()->size(); 
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImagesSize);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImagesSize);

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapChainImagesSize);

        if (vkCreateDescriptorPool(*VulkanDeviceLocator::GetLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to create descriptor pool");
        }
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(*VulkanDeviceLocator::GetLogicalDevice(), descriptorPool, nullptr);
    }

    VkDescriptorPool* VulkanDescriptorPool::GetDescriptorPool()
    {
        return &descriptorPool;
    }
}
