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
