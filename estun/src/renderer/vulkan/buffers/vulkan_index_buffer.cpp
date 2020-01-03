#include "renderer/vulkan/buffers/vulkan_index_buffer.h"
#include "renderer/vulkan/buffers/vulkan_buffer_manager.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include <cstring>


namespace estun
{
    VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t>* indices)
    {
        VkDeviceSize bufferSize = sizeof((*indices)[0]) * indices->size();

        size = indices->size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanBufferManager::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*VulkanDeviceLocator::GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices->data(), (size_t)bufferSize);
        vkUnmapMemory(*VulkanDeviceLocator::GetLogicalDevice(), stagingBufferMemory);

        VulkanBufferManager::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        VulkanBufferManager::CopyBuffer(&stagingBuffer, indexBuffer, bufferSize);

        vkDestroyBuffer(*VulkanDeviceLocator::GetLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(*VulkanDeviceLocator::GetLogicalDevice(), stagingBufferMemory, nullptr);
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        vkDestroyBuffer(*VulkanDeviceLocator::GetLogicalDevice(), indexBuffer, nullptr);
        vkFreeMemory(*VulkanDeviceLocator::GetLogicalDevice(), indexBufferMemory, nullptr);
    }

    VkBuffer* VulkanIndexBuffer::GetBuffer()
    {
        return &indexBuffer;
    }

    uint32_t VulkanIndexBuffer::GetSize()
    {
        return size;
    }
}
