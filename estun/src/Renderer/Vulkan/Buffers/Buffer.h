#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cstring>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class CommandPool;
class DeviceMemory;

class Buffer
{
public:
	Buffer(const Buffer &) = delete;
	Buffer(Buffer &&) = delete;

	Buffer &operator=(const Buffer &) = delete;
	Buffer &operator=(Buffer &&) = delete;

	Buffer(size_t size, VkBufferUsageFlags usage);
	~Buffer();

	DeviceMemory AllocateMemory(VkMemoryPropertyFlags properties);
	VkMemoryRequirements GetMemoryRequirements() const;
	VkDeviceAddress GetDeviceAddress() const;

	void CopyFrom(const Buffer &src, VkDeviceSize size);

	template <class T>
	void CopyFromStagingBuffer(const std::vector<T> &content)
	{
		const auto contentSize = sizeof(content[0]) * content.size();

		auto stagingBuffer = std::make_unique<Buffer>(contentSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		const auto data = stagingBufferMemory.Map(0, contentSize);
		std::memcpy(data, content.data(), contentSize);
		stagingBufferMemory.Unmap();

		CopyFrom(*stagingBuffer, contentSize);

		stagingBuffer.reset();
	}

	VkBuffer GetBuffer() const;

private:
	VkBuffer buffer;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun