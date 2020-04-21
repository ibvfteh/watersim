#define STB_IMAGE_IMPLEMENTATION
#include "Renderer/Vulkan/Material/Texture.h"
#include "Core/Core.h"
#include "Renderer/Vulkan/Material/Image.h"
#include "Renderer/Vulkan/Context/ImageView.h"
#include "Renderer/Vulkan/Material/Sampler.h"
#include "Renderer/Vulkan/DeviceMemory.h"
#include "Renderer/Vulkan/Material/Descriptable.h"
#include "Renderer/Vulkan/Context/CommandPool.h"
#include "Renderer/Vulkan/Buffers/Buffer.h"

Estun::Renderer::Vulkan::Texture
Estun::Renderer::Vulkan::Texture::LoadTexture(const std::string &filename, const Estun::Renderer::Vulkan::SamplerConfig &samplerConfig)
{
    // Load the texture in normal host memory.
    int width, height, channels;
    const auto pixels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!pixels)
    {
        ES_CORE_ASSERT("failed to load texture image '" + filename + "'");
    }

    return Texture(width, height, channels, pixels, samplerConfig);
}

Estun::Renderer::Vulkan::Texture::Texture(int width, int height, int channels, unsigned char *const pixels, const Estun::Renderer::Vulkan::SamplerConfig &samplerConfig)
    : samplerConfig_(samplerConfig),
      width_(width),
      height_(height),
      channels_(channels),
      pixels_(pixels, stbi_image_free)
{
}

Estun::Renderer::Vulkan::Texture::~Texture()
{
	sampler_.reset();
	imageView_.reset();
	image_.reset();
	imageMemory_.reset();
}

void Estun::Renderer::Vulkan::Texture::ToGPU(CommandPool &commandPool)
{
    const VkDeviceSize imageSize = width_ * height_ * 4;

    auto stagingBuffer = std::make_unique<Buffer>(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    const auto data = stagingBufferMemory.Map(0, imageSize);
    std::memcpy(data, pixels_.get(), imageSize);
    stagingBufferMemory.Unmap();

    // Create the device side image, memory, view and sampler.
    image_.reset(new Image(static_cast<uint32_t>(width_), static_cast<uint32_t>(height_), VK_FORMAT_R8G8B8A8_UNORM));
    imageMemory_.reset(new DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    imageView_.reset(new ImageView(image_.get(), VK_IMAGE_ASPECT_COLOR_BIT));
    sampler_.reset(new Sampler(samplerConfig_));

    // Transfer the data to device side.
    image_->TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    image_->CopyFrom(*stagingBuffer);
    image_->TransitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Delete the buffer before the memory
    stagingBuffer.reset();
}

Estun::Renderer::Vulkan::DescriptableInfo Estun::Renderer::Vulkan::Texture::GetInfo()
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView_->GetImageView();
    imageInfo.sampler = sampler_->GetSampler();

    DescriptableInfo info;
    info.iI = imageInfo;

    return info;
} 