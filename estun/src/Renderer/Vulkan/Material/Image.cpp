#include "Renderer/Vulkan/Material/Image.h"
#include "Renderer/Vulkan/DeviceMemory.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Context/SingleTimeCommands.h"
#include "Renderer/Vulkan/Context/Resources.h"
#include "Renderer/Vulkan/Buffers/Buffer.h"

#include <stb_image.h>

Estun::Renderer::Vulkan::Image::Image(const uint32_t width, const uint32_t height, VkFormat format)
    : Image(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
{

}

Estun::Renderer::Vulkan::Image::Image(const uint32_t width, const uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
    : Image(width, height, 1, VK_SAMPLE_COUNT_1_BIT, format, tiling, usage)
{
    
}

Estun::Renderer::Vulkan::Image::Image(
    const uint32_t width, const uint32_t height, const uint32_t mipLevels,
    VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage)
    : width_(width),
      height_(height),
      format_(format),
      mipLevels_(mipLevels),
      layout_(VK_IMAGE_LAYOUT_UNDEFINED)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width_;
    imageInfo.extent.height = height_;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels_;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format_;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateImage(DeviceLocator::GetLogicalDevice(), &imageInfo, nullptr, &image), "Failed to create image");
}

Estun::Renderer::Vulkan::Image::Image(Image &&other) noexcept
    : width_(other.width_),
      height_(other.height_),
      format_(other.format_),
      layout_(other.layout_),
      image(other.image)
{
    other.image = nullptr;
}

Estun::Renderer::Vulkan::Image::~Image()
{
    if (image != nullptr)
    {
        vkDestroyImage(DeviceLocator::GetLogicalDevice(), image, nullptr);
        image = nullptr;
    }
}

Estun::Renderer::Vulkan::DeviceMemory Estun::Renderer::Vulkan::Image::AllocateMemory(const VkMemoryPropertyFlags properties) const
{
    const auto requirements = GetMemoryRequirements();
    DeviceMemory memory(requirements.size, requirements.memoryTypeBits, properties);

    VK_CHECK_RESULT(vkBindImageMemory(DeviceLocator::GetLogicalDevice(), image, memory.GetMemory(), 0), "Failed to bind image memory");

    return memory;
}

VkMemoryRequirements Estun::Renderer::Vulkan::Image::GetMemoryRequirements() const
{
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(DeviceLocator::GetLogicalDevice(), image, &requirements);
    return requirements;
}

void Estun::Renderer::Vulkan::Image::TransitionImageLayout(const VkImageLayout &newLayout)
{

    SingleTimeCommands::SubmitTransfer(CommandPoolLocator::GetTransferPool(), [&](VkCommandBuffer commandBuffer) 
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = layout_;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (DepthResources::HasStencilComponent(format_))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (layout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (layout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (layout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            ES_CORE_ASSERT("Unsupported layout transition");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    });

    layout_ = newLayout;
}

void Estun::Renderer::Vulkan::Image::CopyFrom(const Buffer &buffer)
{
    SingleTimeCommands::SubmitTransfer(CommandPoolLocator::GetTransferPool(), [&](VkCommandBuffer commandBuffer) 
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
        region.imageExtent = {width_, height_, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer.GetBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    });
}

void Estun::Renderer::Vulkan::Image::GenerateMipmaps()
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(DeviceLocator::GetPhysicalDevice(), format_, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        ES_CORE_ASSERT("Texture image format does not support linear blitting");
    }

    SingleTimeCommands::SubmitTransfer(CommandPoolLocator::GetTransferPool(), [&](VkCommandBuffer commandBuffer) 
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = width_;
        int32_t mipHeight = height_;

        for (uint32_t i = 1; i < mipLevels_; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels_ - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
    });
}

VkImage Estun::Renderer::Vulkan::Image::GetImage() const
{
    return image;
}

uint32_t Estun::Renderer::Vulkan::Image::GetWidth()
{
    return width_;
}

uint32_t Estun::Renderer::Vulkan::Image::GetHeight()
{
    return height_;
}

VkFormat Estun::Renderer::Vulkan::Image::GetFormat()
{
    return format_;
}

VkImageLayout Estun::Renderer::Vulkan::Image::GetLayout()
{
    return layout_;
}

uint32_t Estun::Renderer::Vulkan::Image::GetMipLevels()
{
    return mipLevels_;
}