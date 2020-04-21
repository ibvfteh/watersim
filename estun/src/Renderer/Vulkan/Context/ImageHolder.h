#pragma once

#include <memory>
#include <string>
#include "Renderer/Vulkan/Material/Descriptable.h"

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Image;
class ImageView;
class DeviceMemory;

class ImageHolder : public Descriptable
{
public:
    ImageHolder(const ImageHolder &) = delete;
    ImageHolder(ImageHolder &&) = delete;
    ImageHolder &operator=(const ImageHolder &) = delete;
    ImageHolder &operator=(ImageHolder &&) = delete;

    ImageHolder(const uint32_t width, const uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags);
    ~ImageHolder();
    
    DescriptableInfo GetInfo() override; 

    const Image &GetImage() const { return *image_; }
    const ImageView &GetImageView() const { return *imageView_; }

private:
    std::unique_ptr<Image> image_;
    std::unique_ptr<DeviceMemory> imageMemory_;
    std::unique_ptr<ImageView> imageView_;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
