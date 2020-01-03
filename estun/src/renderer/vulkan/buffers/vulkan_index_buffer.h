#pragma once

#include <vulkan/vulkan.h>
#include <vector>


namespace estun
{
    class VulkanIndexBuffer
    {
    private:
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t size;
    public:
        VulkanIndexBuffer(std::vector<uint32_t>* indices);
        ~VulkanIndexBuffer();

        VkBuffer* GetBuffer();
        uint32_t GetSize();
    };
}
