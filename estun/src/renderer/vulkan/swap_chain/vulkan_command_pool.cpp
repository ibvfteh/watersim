#include "renderer/vulkan/swap_chain/vulkan_command_pool.h"
#include "renderer/vulkan/context/vulkan_device.h"

namespace estun
{
    VulkanCommandPool::VulkanCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = VulkanDeviceLocator::GetDevice()->GetQueueFamilyIndices();
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(*VulkanDeviceLocator::GetLogicalDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            ES_CORE_ASSERT("Failed to create command pool");
        }
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        vkDestroyCommandPool(*VulkanDeviceLocator::GetLogicalDevice(), commandPool, nullptr);
    }  
/*
    void VulkanCommandPool::ResetCommandPool()
    {
        vkResetCommandPool(*VulkanDeviceLocator::GetLogicalDevice(), commandPool, nullptr);
    }
*/
    VkCommandPool* VulkanCommandPool::GetCommandPool()
    {
        return &commandPool;
    }
}
