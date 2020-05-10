#pragma once

#include "renderer/common.h"

namespace estun
{

class Buffer;
class DeviceMemory;

class Image
{
private:
	VkImage image;
	uint32_t width_;
	uint32_t height_;
	const VkFormat format_;
	VkImageLayout layout_;
	uint32_t mipLevels_;

public:
	Image(const Image &) = delete;
	Image(Image &&other) noexcept;

	Image &operator=(const Image &) = delete;
	Image &operator=(Image &&) = delete;

	Image(const uint32_t width, const uint32_t height, VkFormat format);
	Image(const uint32_t width, const uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
	Image(const uint32_t width, const uint32_t height, const uint32_t mipLevels,
		  VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
		  VkImageUsageFlags usage);

	~Image();

	DeviceMemory AllocateMemory(VkMemoryPropertyFlags properties) const;
	VkMemoryRequirements GetMemoryRequirements() const;

	void TransitionImageLayout(const VkImageLayout &newLayout);
	void CopyFrom(const Buffer &buffer);
	void GenerateMipmaps();

	VkImage GetImage() const;
	uint32_t GetWidth();
	uint32_t GetHeight();
	VkFormat GetFormat();
	VkImageLayout GetLayout();
	uint32_t GetMipLevels();

	void SetLayout(VkImageLayout layout);
};

} // namespace estun
