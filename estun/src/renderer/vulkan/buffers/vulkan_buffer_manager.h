#pragma once

#include <vulkan/vulkan.h>
#include <memory>

namespace estun
{
    class VulkanBufferManager
	{
	public:
		static void CreateBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage,
		                         const VkMemoryPropertyFlags& properties, VkBuffer& buffer,
		                         VkDeviceMemory& bufferMemory);

		static void CopyBuffer(VkBuffer* srcBuffer, VkBuffer& dstBuffer, const VkDeviceSize& size,
		                       const VkDeviceSize& scrOffset = 0, const VkDeviceSize& dstOffset = 0,
		                       uint32_t regionCount = 1);

		static void CopyBufferToImage(VkBuffer* buffer, VkImage* image,
		                              const uint32_t width, const uint32_t height);

		static void CopyBufferToImage(VkBuffer* buffer, VkImage* image,
		                              VkBufferImageCopy* region, int regioncount,
		                              VkImageLayout layout);

		static std::unique_ptr<VkCommandBuffer> BeginSingleTimeCommands();

		static void EndSingleTimeCommands(const std::unique_ptr<VkCommandBuffer>& commandBuffer);
	};
}
