#pragma once

#include "renderer/common.h"
#include "renderer/material/descriptable.h"

namespace estun
{

    class Image;
    class ImageView;
    class DeviceMemory;
    class Sampler;

    class ImageHolder : public Descriptable
    {
    public:
        ImageHolder(const ImageHolder &) = delete;
        ImageHolder(ImageHolder &&) = delete;
        ImageHolder &operator=(const ImageHolder &) = delete;
        ImageHolder &operator=(ImageHolder &&) = delete;

        ImageHolder(const uint32_t width, const uint32_t height,
                    VkSampleCountFlagBits numSamples, VkFormat format,
                    VkImageTiling tiling, VkImageUsageFlags usage,
                    VkImageAspectFlags aspectFlags);
        ~ImageHolder();

        DescriptableInfo GetInfo() override;
        void ToLayout(const VkImageLayout &newLayout);
        void Barrier(
            VkCommandBuffer &commandBuffer,
            const VkImageLayout &newLayout,
            VkAccessFlags srcAccessMask,
            VkAccessFlags dstAccessMask,
            VkPipelineStageFlags sourceStage,
            VkPipelineStageFlags destinationStage);
        void CopyTo(
            VkCommandBuffer &commandBuffer,
            std::shared_ptr<ImageHolder> imageH);

        const Image &GetImage() const  { return *image_; }
        const std::unique_ptr<ImageView> &GetImageView() const { return imageView_; }
        const Sampler &GetSampler() const { return *sampler_; }

    protected:
        VkImageLayout layout_;
        std::unique_ptr<Image> image_;
        std::unique_ptr<DeviceMemory> imageMemory_;
        std::unique_ptr<ImageView> imageView_;
        std::unique_ptr<Sampler> sampler_;
    };

} // namespace estun
