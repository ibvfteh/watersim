#include "Renderer/Vulkan/DeviceMemory.h"
#include <Renderer/Vulkan/Context/Device.h>

Estun::Renderer::Vulkan::DeviceMemory::DeviceMemory(
	const size_t size, 
	const uint32_t memoryTypeBits, 
	const VkMemoryPropertyFlags properties) 
{
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = size;
	allocInfo.memoryTypeIndex = FindMemoryType(memoryTypeBits, properties);

	VK_CHECK_RESULT(vkAllocateMemory(DeviceLocator::GetLogicalDevice(), &allocInfo, nullptr, &memory), "Failed to allocate memory");
}

Estun::Renderer::Vulkan::DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept :
	memory(other.memory)
{
	other.memory = nullptr;
}

Estun::Renderer::Vulkan::DeviceMemory::~DeviceMemory()
{
	if (memory != nullptr)
	{
		vkFreeMemory(DeviceLocator::GetLogicalDevice(), memory, nullptr);
		memory = nullptr;
	}
}

void* Estun::Renderer::Vulkan::DeviceMemory::Map(const size_t offset, const size_t size)
{
	void* data;
	VK_CHECK_RESULT(vkMapMemory(DeviceLocator::GetLogicalDevice(), memory, offset, size, 0, &data), "Failed to map memory");

	return data;
}

void Estun::Renderer::Vulkan::DeviceMemory::Unmap()
{
	vkUnmapMemory(DeviceLocator::GetLogicalDevice(), memory);
}

VkDeviceMemory Estun::Renderer::Vulkan::DeviceMemory::GetMemory() const
{
	return memory;
}

uint32_t Estun::Renderer::Vulkan::DeviceMemory::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(DeviceLocator::GetPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i != memProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (memProperties.memoryTypes[i].propertyFlags == properties)
        {
            return i;
        }
    }

    ES_CORE_ASSERT("Failed to find suitable memory type");
    return 0;
}