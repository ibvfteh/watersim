#include "Renderer/Vulkan/Context/ImageHolder.h"
#include "Renderer/Vulkan/Material/Image.h"
#include "Renderer/Vulkan/Context/ImageView.h"
#include "Renderer/Vulkan/DeviceMemory.h"
#include "Renderer/Vulkan/Material/Descriptable.h"

Estun::Renderer::Vulkan::ImageHolder::ImageHolder(
    const uint32_t width,
    const uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkImageAspectFlags aspectFlags)
{
	image_.reset(new Image(width, height, format, tiling, usage));
	imageMemory_.reset(new DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
	imageView_.reset(new ImageView(image_.get(), aspectFlags));
}

Estun::Renderer::Vulkan::ImageHolder::~ImageHolder()
{
	image_.reset();
	imageMemory_.reset();
	imageView_.reset();
}


Estun::Renderer::Vulkan::DescriptableInfo Estun::Renderer::Vulkan::ImageHolder::GetInfo()
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = imageView_->GetImageView();

    DescriptableInfo info;
    info.iI = imageInfo;

    return info;
} 