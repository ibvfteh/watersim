#include "renderer/context/image_holder.h"
#include "renderer/context/image.h"
#include "renderer/context/image_view.h"
#include "renderer/device_memory.h"
#include "renderer/material/sampler.h"
#include "renderer/material/descriptable.h"

estun::ImageHolder::ImageHolder(
    const uint32_t width,
    const uint32_t height,
    VkSampleCountFlagBits numSamples,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageAspectFlags aspectFlags)
    : layout_(VK_IMAGE_LAYOUT_UNDEFINED)
{
    image_.reset(new Image(width, height, 1, numSamples, format, tiling, usage));
    imageMemory_.reset(new DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    imageView_.reset(new ImageView(image_.get(), aspectFlags));
    //TODO: позволять фильтрацию
    SamplerConfig samplerConfig;
    samplerConfig.AnisotropyEnable = false;
    sampler_.reset(new Sampler(samplerConfig));
}

estun::ImageHolder::~ImageHolder()
{
    image_.reset();
    imageMemory_.reset();
    imageView_.reset();
    sampler_.reset();
}

void estun::ImageHolder::ToLayout(const VkImageLayout &newLayout)
{
    layout_ = newLayout;
    image_->TransitionImageLayout(newLayout);
}

void estun::ImageHolder::Barrier(
    VkCommandBuffer &commandBuffer,
    const VkImageLayout &newLayout,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkPipelineStageFlags sourceStage,
    VkPipelineStageFlags destinationStage)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = layout_;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image_->GetImage();
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;
    layout_ = newLayout;
    image_->SetLayout(newLayout);

    sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void estun::ImageHolder::CopyTo(
    VkCommandBuffer &commandBuffer,
    std::shared_ptr<ImageHolder> imageH)

{
    VkImageCopy imageCopy = {};
    imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    imageCopy.srcOffset = {0, 0, 0};
    imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    imageCopy.dstOffset = {0, 0, 0};
    imageCopy.extent = {image_->GetWidth(), image_->GetHeight(), 1};

    vkCmdCopyImage(
        commandBuffer,
        image_->GetImage(), layout_,
        imageH->GetImage().GetImage(), imageH->layout_,
        1, &imageCopy);
}
estun::DescriptableInfo estun::ImageHolder::GetInfo()
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = layout_;
    imageInfo.imageView = imageView_->GetImageView();
    imageInfo.sampler = sampler_->GetSampler();

    DescriptableInfo info;
    info.iI = imageInfo;

    return info;
}