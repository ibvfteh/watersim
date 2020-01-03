#include "renderer/vulkan/context/vulkan_semaphores.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include "core/core.h"

namespace estun
{
    VulkanSemaphoresManager::VulkanSemaphoresManager(const uint32_t imageCount_t)
        : imageCount(imageCount_t)
    {
        imageAvailableSemaphores.resize(imageCount);
        renderFinishedSemaphores.resize(imageCount);
        inFlightFences.resize(imageCount);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < imageCount; i++) 
        {
            if (vkCreateSemaphore(*VulkanDeviceLocator::GetLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(*VulkanDeviceLocator::GetLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(*VulkanDeviceLocator::GetLogicalDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) 
            {
                ES_CORE_ASSERT("Failed to create synchronization objects for a frame");
            }
        }
    }

    VulkanSemaphoresManager::~VulkanSemaphoresManager()
    {
        for (size_t i = 0; i < imageCount; i++) 
        {
            vkDestroySemaphore(*VulkanDeviceLocator::GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(*VulkanDeviceLocator::GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(*VulkanDeviceLocator::GetLogicalDevice(), inFlightFences[i], nullptr);
        }
    }

    const std::vector<VkSemaphore>* VulkanSemaphoresManager::GetImageAvailableSemaphores() const
    {
        return &imageAvailableSemaphores;
    }

    const std::vector<VkSemaphore>* VulkanSemaphoresManager::GetRenderFinishedSemaphores() const
    {
        return &renderFinishedSemaphores;
    }

    const std::vector<VkFence>* VulkanSemaphoresManager::GetInFlightFences() const
    {
        return &inFlightFences;
    }
}
