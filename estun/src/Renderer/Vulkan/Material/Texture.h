#pragma once

#include <memory>
#include <string>
#include "stb_image.h"
#include "Renderer/Vulkan/Material/Descriptable.h"
#include "Renderer/Vulkan/Material/Sampler.h"

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Image;
class ImageView;
class Sampler;
class DeviceMemory;
class CommandPool;
class SamplerConfig;

class Texture : public Descriptable
{
public:
    Texture(const Texture &) = delete;
    Texture(Texture &&) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&) = delete;

    Texture(int width, int height, int channels, unsigned char* const pixels, const SamplerConfig &samplerConfig);
    ~Texture();
    
    DescriptableInfo GetInfo() override; 

    static Texture LoadTexture(const std::string& filename, const SamplerConfig& samplerConfig);
    void ToGPU(CommandPool &commandPool);
    const ImageView &GetImageView() const { return *imageView_; }
    const Sampler &GetSampler() const { return *sampler_; }

private:
    SamplerConfig samplerConfig_;
    int width_;
    int height_;
    int channels_;
    std::unique_ptr<unsigned char, void (*)(void *)> pixels_;

    std::unique_ptr<Image> image_;
    std::unique_ptr<DeviceMemory> imageMemory_;
    std::unique_ptr<ImageView> imageView_;
    std::unique_ptr<Sampler> sampler_;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun