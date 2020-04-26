#include "renderer/buffers/uniform_buffer.h"
#include "renderer/device_memory.h"
#include "renderer/buffers/buffer.h"
#include "renderer/material/descriptable.h"

estun::UniformBuffer::UniformBuffer()
{
    const auto bufferSize = sizeof(UniformBufferObject);

    buffer.reset(new Buffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    memory.reset(new DeviceMemory(buffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
}

estun::UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept : buffer(other.buffer.release()),
                                                                                        memory(other.memory.release())
{
}

estun::UniformBuffer::~UniformBuffer()
{
    buffer.reset();
    memory.reset();
}

void estun::UniformBuffer::SetValue(const UniformBufferObject &ubo)
{
    const auto data = memory->Map(0, sizeof(UniformBufferObject));
    std::memcpy(data, &ubo, sizeof(ubo));
    memory->Unmap();
}

const estun::Buffer &estun::UniformBuffer::GetBuffer() const 
{ 
    return *buffer; 
}

estun::DescriptableInfo estun::UniformBuffer::GetInfo()
{
    VkDescriptorBufferInfo uniformBufferInfo = {};
    uniformBufferInfo.buffer = buffer->GetBuffer();
    uniformBufferInfo.range = VK_WHOLE_SIZE;
    
    DescriptableInfo info;
    info.bI = uniformBufferInfo;

    return info;
} 

/*
estun::UniformBuffer::UniformBuffer()
{
    size_t swapChainImagesSize = ContextLocator::GetContext()->GetSwapChain()->GetSwapChainImagesVector()->size();

    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(swapChainImagesSize);
    uniformBuffersMemory.resize(swapChainImagesSize);

    for (size_t i = 0; i < swapChainImagesSize; i++)
    {
        BufferManager::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
    }

    mappedMemory.resize(swapChainImagesSize);
    for (size_t i = 0; i < swapChainImagesSize; i++)
    {
        vkMapMemory(*DeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &mappedMemory[i]);
    }
}

estun::UniformBuffer::~UniformBuffer()
{
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        vkDestroyBuffer(*DeviceLocator::GetLogicalDevice(), uniformBuffers[i], nullptr);
        vkUnmapMemory(*DeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i]);
        vkFreeMemory(*DeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void estun::UniformBuffer::Update(estun::UniformBufferObject &newUbo)
{
    uint32_t currentImage = ContextLocator::GetContext()->GetCurrentImage();
    ubo = newUbo;
    std::memcpy(mappedMemory[currentImage], &ubo, sizeof(ubo));
}

std::vector<VkBuffer> *estun::UniformBuffer::GetUniformBuffers()
{
    return &uniformBuffers;
}

std::vector<VkDeviceMemory> *estun::UniformBuffer::GetUniformBuffersMemory()
{
    return &uniformBuffersMemory;
}
*/