#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "Renderer/Vulkan/Context.h"
#include "Renderer/Vulkan/Context/CommandBuffers.h"
#include "Renderer/Vulkan/Buffers/StorageBuffer.h"
#include "Renderer/Vulkan/DeviceMemory.h"
#include "Renderer/Vulkan/Buffers/Buffer.h"
#include "Renderer/Vulkan/Material/Descriptable.h"
#include "Renderer/Vulkan/Buffers/Vertex.h"

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

//class Vertex;
//class Buffer;
//class DeviceMemory;

template <class T>
class StorageBuffer : public Descriptable
{
public:
    StorageBuffer(const StorageBuffer &) = delete;
    StorageBuffer &operator=(const StorageBuffer &) = delete;
    StorageBuffer &operator=(StorageBuffer &&) = delete;

    StorageBuffer(const std::vector<T> &storage)
    {
        const auto bufferSize = sizeof(storage[0]) * storage.size();
        buffer.reset(new Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
        memory.reset(new DeviceMemory(buffer->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
        buffer->CopyFromStagingBuffer<T>(storage);
    }

    StorageBuffer(StorageBuffer &&other) noexcept
        : buffer(other.buffer.release()),
          memory(other.memory.release())
    {
    }

    ~StorageBuffer()
    {
        buffer.reset();
        memory.reset();
    }

    DescriptableInfo GetInfo() override
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = buffer->GetBuffer();
        bufferInfo.range = VK_WHOLE_SIZE;

        DescriptableInfo info;
        info.bI = bufferInfo;

        return info;
    }

    const Buffer &GetBuffer() const
    {
        return *buffer;
    }

protected:
    std::unique_ptr<Buffer> buffer;
    std::unique_ptr<DeviceMemory> memory;
};

class VertexBuffer : public StorageBuffer<Vertex>
{
public:
    VertexBuffer(const std::vector<Vertex> &storage) : StorageBuffer(storage){}
    void Bind()
    {
        VkBuffer vertexBuffers[] = {buffer->GetBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(ContextLocator::GetCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);
    }
};

class IndexBuffer : public StorageBuffer<uint32_t>
{
public:
    IndexBuffer(const std::vector<uint32_t> &storage) : StorageBuffer(storage){}
    void Bind()
    {
        const VkBuffer indexBuffer = buffer->GetBuffer();
        vkCmdBindIndexBuffer(ContextLocator::GetCurrentCommandBuffer(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
