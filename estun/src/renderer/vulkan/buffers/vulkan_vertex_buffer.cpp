#include "renderer/vulkan/buffers/vulkan_vertex_buffer.h"
#include "renderer/vulkan/buffers/vulkan_buffer_manager.h"
#include "renderer/vulkan/context/vulkan_device.h"
namespace estun
{
    VulkanVertexBuffer::VulkanVertexBuffer(std::vector<Vertex>* vertices)
    {
        bufferSize = sizeof((*vertices)[0]) * vertices->size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanBufferManager::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*VulkanDeviceLocator::GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices->data(), (size_t)bufferSize);
        vkUnmapMemory(*VulkanDeviceLocator::GetLogicalDevice(), stagingBufferMemory);

        VulkanBufferManager::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        VulkanBufferManager::CopyBuffer(&stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(*VulkanDeviceLocator::GetLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(*VulkanDeviceLocator::GetLogicalDevice(), stagingBufferMemory, nullptr);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        vkDestroyBuffer(*VulkanDeviceLocator::GetLogicalDevice(), vertexBuffer, nullptr);
        vkFreeMemory(*VulkanDeviceLocator::GetLogicalDevice(), vertexBufferMemory, nullptr);
    }

    VkBuffer* VulkanVertexBuffer::GetBuffer()
    {
        return &vertexBuffer;
    }

    VkDeviceSize VulkanVertexBuffer::GetSize()
    {
        return bufferSize;
    }
}
