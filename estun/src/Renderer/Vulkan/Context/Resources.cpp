#include "Renderer/Vulkan/Context/Resources.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Material/Image.h"
#include "Renderer/Vulkan/Context/ImageView.h"
#include "Renderer/Vulkan/Context/SwapChain.h"
#include "Renderer/Vulkan/DeviceMemory.h"

Estun::Renderer::Vulkan::DepthResources::DepthResources(const VkExtent2D &swapChainExtent, VkSampleCountFlagBits msaaSamples)
{
    VkFormat depthFormat = FindDepthFormat();
    image_.reset(new Image(
        swapChainExtent.width,
        swapChainExtent.height,
        1,
        msaaSamples,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
    imageMemory_.reset(new DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    imageView_.reset(new ImageView(image_.get(), VK_IMAGE_ASPECT_DEPTH_BIT));
}

Estun::Renderer::Vulkan::DepthResources::~DepthResources()
{
    image_.reset();
    imageMemory_.reset();
    imageView_.reset();
}

VkFormat Estun::Renderer::Vulkan::DepthResources::FindDepthFormat()
{
    return FindSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat Estun::Renderer::Vulkan::DepthResources::FindSupportedFormat(
    const std::vector<VkFormat> &candidates,
    VkImageTiling tiling,
    const VkFormatFeatureFlags &features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(DeviceLocator::GetPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }

        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    ES_CORE_ASSERT("Failed to find supported format");
}

bool Estun::Renderer::Vulkan::DepthResources::HasStencilComponent(const VkFormat &format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkImage Estun::Renderer::Vulkan::DepthResources::GetDepthImage()
{
    return image_->GetImage();
}

VkDeviceMemory Estun::Renderer::Vulkan::DepthResources::GetDepthImageMemory()
{
    return imageMemory_->GetMemory();
}

VkImageView Estun::Renderer::Vulkan::DepthResources::GetDepthImageView()
{
    return imageView_->GetImageView();
}

Estun::Renderer::Vulkan::ColorResources::ColorResources(Estun::Renderer::Vulkan::SwapChain *swapChain, VkSampleCountFlagBits msaaSamples)
{
    VkFormat colorFormat = swapChain->GetFormat();
    image_.reset(new Image(
        swapChain->GetExtent().width,
        swapChain->GetExtent().height,
        1,
        msaaSamples,
        colorFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT));
    imageMemory_.reset(new DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    imageView_.reset(new ImageView(image_.get(), VK_IMAGE_ASPECT_COLOR_BIT));
}

Estun::Renderer::Vulkan::ColorResources::~ColorResources()
{
    image_.reset();
    imageMemory_.reset();
    imageView_.reset();
}

VkImage Estun::Renderer::Vulkan::ColorResources::GetColorImage()
{
    return image_->GetImage();
}

VkDeviceMemory Estun::Renderer::Vulkan::ColorResources::GetColorImageMemory()
{
    return imageMemory_->GetMemory();
}

VkImageView Estun::Renderer::Vulkan::ColorResources::GetColorImageView()
{
    return imageView_->GetImageView();
}
