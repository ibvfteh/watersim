#include "Renderer/Vulkan/Buffers/Buffer.h"
#include "Renderer/Vulkan/Context/SingleTimeCommands.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/DeviceMemory.h"
#include "Renderer/Vulkan/Context/CommandPool.h"


Estun::Renderer::Vulkan::Buffer::Buffer(const size_t size, const VkBufferUsageFlags usage)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateBuffer(DeviceLocator::GetLogicalDevice(), &bufferInfo, nullptr, &buffer), "Failed to create buffer");
}

Estun::Renderer::Vulkan::Buffer::~Buffer()
{
    if (buffer != nullptr)
    {
        vkDestroyBuffer(DeviceLocator::GetLogicalDevice(), buffer, nullptr);
        buffer = nullptr;
    }
}

Estun::Renderer::Vulkan::DeviceMemory Estun::Renderer::Vulkan::Buffer::AllocateMemory(const VkMemoryPropertyFlags properties)
{
    const auto requirements = GetMemoryRequirements();
    DeviceMemory memory(requirements.size, requirements.memoryTypeBits, properties);

    VK_CHECK_RESULT(vkBindBufferMemory(DeviceLocator::GetLogicalDevice(), buffer, memory.GetMemory(), 0), "Failed to bind buffer memory");

    return memory;
}

VkMemoryRequirements Estun::Renderer::Vulkan::Buffer::GetMemoryRequirements() const
{
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(DeviceLocator::GetLogicalDevice(), buffer, &requirements);
    return requirements;
}

void Estun::Renderer::Vulkan::Buffer::CopyFrom(const Buffer &src, VkDeviceSize size)
{
    SingleTimeCommands::SubmitTransfer(CommandPoolLocator::GetTransferPool(), [&](VkCommandBuffer commandBuffer) {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;

        vkCmdCopyBuffer(commandBuffer, src.GetBuffer(), GetBuffer(), 1, &copyRegion);
    });
}

VkDeviceAddress Estun::Renderer::Vulkan::Buffer::GetDeviceAddress() const
{
    VkBufferDeviceAddressInfo info;
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.pNext = nullptr;
    info.buffer = GetBuffer();
    VkDeviceAddress deviceAddress = vkGetBufferDeviceAddress(DeviceLocator::GetLogicalDevice(), &info);
    return deviceAddress;
}

VkBuffer Estun::Renderer::Vulkan::Buffer::GetBuffer() const
{
    return buffer;
}