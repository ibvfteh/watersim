#include "renderer/vulkan/buffers/vulkan_uniform_buffer.h"
#include "renderer/vulkan/buffers/vulkan_buffer_manager.h"
#include "renderer/vulkan/context/vulkan_device.h"
#include "renderer/vulkan/vulkan_context.h"

namespace estun
{
    VulkanUniformBuffer::VulkanUniformBuffer()
    {
        size_t swapChainImagesSize = VulkanContextLocator::GetContext()->GetSwapChain()->GetSwapChainImagesVector()->size();

        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(swapChainImagesSize);
        uniformBuffersMemory.resize(swapChainImagesSize);

        for (size_t i = 0; i < swapChainImagesSize; i++) 
        {
            VulkanBufferManager::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        }
        
        mappedMemory.resize(swapChainImagesSize);
        for (size_t i = 0; i < swapChainImagesSize; i++)
        {
            vkMapMemory(*VulkanDeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &mappedMemory[i]);
        }
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            vkDestroyBuffer(*VulkanDeviceLocator::GetLogicalDevice(), uniformBuffers[i], nullptr);
            vkUnmapMemory(*VulkanDeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i]);
            vkFreeMemory(*VulkanDeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i], nullptr);
        }
    }

    void VulkanUniformBuffer::Update(UniformBufferObject& newUbo)
    {
        uint32_t currentImage = VulkanContextLocator::GetContext()->GetCurrentImage();
        ubo = newUbo;
        std::memcpy(mappedMemory[currentImage], &ubo, sizeof(ubo));
    }

    std::vector<VkBuffer>* VulkanUniformBuffer::GetUniformBuffers() 
    {
        return &uniformBuffers;
    }

    std::vector<VkDeviceMemory>* VulkanUniformBuffer::GetUniformBuffersMemory() 
    {
        return &uniformBuffersMemory;
    }
}
