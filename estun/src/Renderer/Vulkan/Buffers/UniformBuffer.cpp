#include "Renderer/Vulkan/Buffers/UniformBuffer.h"
#include "Renderer/Vulkan/DeviceMemory.h"
#include "Renderer/Vulkan/Buffers/Buffer.h"
#include "Renderer/Vulkan/Material/Descriptable.h"

Estun::Renderer::Vulkan::UniformBuffer::UniformBuffer()
{
    const auto bufferSize = sizeof(UniformBufferObject);

    buffer.reset(new Buffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    memory.reset(new DeviceMemory(buffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
}

Estun::Renderer::Vulkan::UniformBuffer::UniformBuffer(UniformBuffer &&other) noexcept : buffer(other.buffer.release()),
                                                                                        memory(other.memory.release())
{
}

Estun::Renderer::Vulkan::UniformBuffer::~UniformBuffer()
{
    buffer.reset();
    memory.reset();
}

void Estun::Renderer::Vulkan::UniformBuffer::SetValue(const UniformBufferObject &ubo)
{
    const auto data = memory->Map(0, sizeof(UniformBufferObject));
    std::memcpy(data, &ubo, sizeof(ubo));
    memory->Unmap();
}

const Estun::Renderer::Vulkan::Buffer &Estun::Renderer::Vulkan::UniformBuffer::GetBuffer() const 
{ 
    return *buffer; 
}

Estun::Renderer::Vulkan::DescriptableInfo Estun::Renderer::Vulkan::UniformBuffer::GetInfo()
{
    VkDescriptorBufferInfo uniformBufferInfo = {};
    uniformBufferInfo.buffer = buffer->GetBuffer();
    uniformBufferInfo.range = VK_WHOLE_SIZE;
    
    DescriptableInfo info;
    info.bI = uniformBufferInfo;

    return info;
} 

/*
Estun::Renderer::Vulkan::UniformBuffer::UniformBuffer()
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

Estun::Renderer::Vulkan::UniformBuffer::~UniformBuffer()
{
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        vkDestroyBuffer(*DeviceLocator::GetLogicalDevice(), uniformBuffers[i], nullptr);
        vkUnmapMemory(*DeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i]);
        vkFreeMemory(*DeviceLocator::GetLogicalDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void Estun::Renderer::Vulkan::UniformBuffer::Update(Estun::Renderer::Vulkan::UniformBufferObject &newUbo)
{
    uint32_t currentImage = ContextLocator::GetContext()->GetCurrentImage();
    ubo = newUbo;
    std::memcpy(mappedMemory[currentImage], &ubo, sizeof(ubo));
}

std::vector<VkBuffer> *Estun::Renderer::Vulkan::UniformBuffer::GetUniformBuffers()
{
    return &uniformBuffers;
}

std::vector<VkDeviceMemory> *Estun::Renderer::Vulkan::UniformBuffer::GetUniformBuffersMemory()
{
    return &uniformBuffersMemory;
}
*/