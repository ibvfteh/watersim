#include "renderer/vulkan/vulkan_memory_manager.h"

namespace estun
{
    uint32_t MemoryManager::FindMemoryType(VulkanDevice* device, const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(*device->GetPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if (memProperties.memoryTypes[i].propertyFlags == properties)
            {
                return i;
            }
        }
        
        ES_CORE_ASSERT("Failed to find suitable memory type");
        return 0;
    }
}