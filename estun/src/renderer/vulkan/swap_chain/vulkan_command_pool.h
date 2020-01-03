#pragma once

#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanCommandPool
    {
    private: 
        VkCommandPool commandPool;

    public:
        VulkanCommandPool();
        ~VulkanCommandPool();

        VkCommandPool* GetCommandPool();

        //void ResetCommandPool();  
    };
}
