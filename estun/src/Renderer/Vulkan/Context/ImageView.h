#pragma once

#include <vulkan/vulkan.h>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Image;

class ImageView
{
public:
    ImageView(const ImageView &) = delete;
    ImageView(ImageView &&) = delete;

    ImageView &operator=(const ImageView &) = delete;
    ImageView &operator=(ImageView &&) = delete;

	explicit ImageView(VkImage *image, const uint32_t mipLevels);
	explicit ImageView(const VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	explicit ImageView(Image* image, VkImageAspectFlags aspectFlags);

	~ImageView();

	VkImageView GetImageView() const;

private:
	VkImageView imageView;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
