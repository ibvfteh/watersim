#pragma once

#include "renderer/common.h"
#include "renderer/context/image_holder.h"

namespace estun
{

class SwapChain;

class DepthResources : public ImageHolder
{
public:
	DepthResources(const VkExtent2D &extent, VkSampleCountFlagBits msaaSamples);
	~DepthResources();

	static VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, const VkFormatFeatureFlags &features);
	static VkFormat FindDepthFormat();
	static bool HasStencilComponent(const VkFormat &format);
};

class ColorResources : public ImageHolder
{
public:
	ColorResources(const VkExtent2D &extent, VkSampleCountFlagBits msaaSamples);
	~ColorResources();
};

} // namespace estun
