#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Image;
class ImageView;
class DeviceMemory;
class SwapChain;

class DepthResources
{
private:
	std::unique_ptr<Image> image_;
	std::unique_ptr<ImageView> imageView_;
	std::unique_ptr<DeviceMemory> imageMemory_;

public:
	DepthResources(const VkExtent2D &swapChainExtent, VkSampleCountFlagBits msaaSamples);
	~DepthResources();

	static VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, const VkFormatFeatureFlags &features);
	static VkFormat FindDepthFormat();
	static bool HasStencilComponent(const VkFormat &format);

	VkImage GetDepthImage();
	VkDeviceMemory GetDepthImageMemory();
	VkImageView GetDepthImageView();
};

class ColorResources
{
private:
	std::unique_ptr<Image> image_;
	std::unique_ptr<ImageView> imageView_;
	std::unique_ptr<DeviceMemory> imageMemory_;

public:
	ColorResources(SwapChain *swapChain, VkSampleCountFlagBits msaaSamples);
	~ColorResources();

	VkImage GetColorImage();
	VkDeviceMemory GetColorImageMemory();
	VkImageView GetColorImageView();
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
