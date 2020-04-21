#pragma once

#include <vulkan/vulkan.hpp>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class DeviceMemory
{
public:
	DeviceMemory(const DeviceMemory &) = delete;
	DeviceMemory &operator=(const DeviceMemory &) = delete;
	DeviceMemory &operator=(DeviceMemory &&) = delete;

	DeviceMemory(size_t size, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
	DeviceMemory(DeviceMemory &&other) noexcept;
	~DeviceMemory();

	void *Map(size_t offset, size_t size);
	void Unmap();

	VkDeviceMemory GetMemory() const;

private:
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	VkDeviceMemory memory;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun