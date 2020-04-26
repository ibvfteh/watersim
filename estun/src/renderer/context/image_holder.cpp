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


estun::DescriptableInfo estun::ImageHolder::GetInfo()
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = imageView_->GetImageView();

    DescriptableInfo info;
    info.iI = imageInfo;

    return info;
} 