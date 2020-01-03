#include "renderer/vulkan/buffers/vulkan_buffer_manager.h"
#include "renderer/vulkan/context/vulkan_device.h"
#include "renderer/vulkan/vulkan_context.h"
#include "renderer/vulkan/vulkan_memory_manager.h"

namespace estun
{
    void VulkanBufferManager::CreateBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage,
	                         const VkMemoryPropertyFlags& properties, VkBuffer& buffer,
	                         VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(*VulkanDeviceLocator::GetLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
        {
            ES_CORE_ASSERT("Failed to create buffer");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(*VulkanDeviceLocator::GetLogicalDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = MemoryManager::FindMemoryType(VulkanDeviceLocator::GetDevice(), memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(*VulkanDeviceLocator::GetLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            ES_CORE_ASSERT("Failed to allocate buffer memory");
        }

        vkBindBufferMemory(*VulkanDeviceLocator::GetLogicalDevice(), buffer, bufferMemory, 0);
    }

	void VulkanBufferManager::CopyBuffer(VkBuffer* srcBuffer, VkBuffer& dstBuffer, const VkDeviceSize& size,
	                       const VkDeviceSize& scrOffset, const VkDeviceSize& dstOffset,
	                       uint32_t regionCount)
    {
    	const std::unique_ptr<VkCommandBuffer> commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        copyRegion.srcOffset = scrOffset;
        copyRegion.dstOffset = dstOffset;
        vkCmdCopyBuffer(*commandBuffer, *srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }
	
    void VulkanBufferManager::CopyBufferToImage(VkBuffer* buffer, VkImage* image,
		                            const uint32_t width, const uint32_t height)
    {
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        CopyBufferToImage(buffer, image, &region, 1, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }

	void VulkanBufferManager::CopyBufferToImage(VkBuffer* buffer, VkImage* image,
	                              VkBufferImageCopy* region, int regionCount,
	                              VkImageLayout layout)
    {
        const std::unique_ptr<VkCommandBuffer> commandBuffer = BeginSingleTimeCommands();

        vkCmdCopyBufferToImage(*commandBuffer, *buffer, *image, layout, regionCount, region);

        EndSingleTimeCommands(commandBuffer);
    }

	std::unique_ptr<VkCommandBuffer> VulkanBufferManager::BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = *VulkanContextLocator::GetContext()->GetCommandPool()->GetCommandPool();
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer* commandBuffer = new VkCommandBuffer();
        vkAllocateCommandBuffers(*VulkanDeviceLocator::GetLogicalDevice(), &allocInfo, commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(*commandBuffer, &beginInfo);

        return std::unique_ptr<VkCommandBuffer>(commandBuffer);
    }

	void VulkanBufferManager::EndSingleTimeCommands(const std::unique_ptr<VkCommandBuffer>& commandBuffer)
    {
        vkEndCommandBuffer(*commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer.get();

        vkQueueSubmit(*VulkanDeviceLocator::GetDevice()->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(*VulkanDeviceLocator::GetDevice()->GetGraphicsQueue());

        vkFreeCommandBuffers(*VulkanDeviceLocator::GetLogicalDevice(), *VulkanContextLocator::GetContext()->GetCommandPool()->GetCommandPool(), 1, commandBuffer.get());
    }
}
