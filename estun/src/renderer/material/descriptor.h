#pragma once

#include "renderer/common.h"
#include "renderer/material/descriptor_sets.h"

namespace estun
{

class DescriptorBinding;
class DescriptorSets;
class DescriptorPool;
class DescriptorSetLayout;
class PipelineLayout;
class TopLevelAccelerationStructure;
class Buffer;
class Texture;

class Descriptor
{
public:
    Descriptor(const Descriptor &) = delete;
    Descriptor(Descriptor &&) = delete;

    Descriptor &operator=(const Descriptor &) = delete;
    Descriptor &operator=(Descriptor &&) = delete;

    explicit Descriptor(const std::vector<DescriptorBinding> &descriptorBindings, size_t maxSets);
    ~Descriptor();

    void Bind(VkCommandBuffer & commandBuffer);

    DescriptorSetLayout &GetDescriptorSetLayout() const;
    DescriptorSets &GetDescriptorSets();
    PipelineLayout &GetPipelineLayout() const;

private:
    std::unique_ptr<DescriptorPool> descriptorPool;
    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<DescriptorSets> descriptorSets;
    std::unique_ptr<PipelineLayout> pipelineLayout;
};

} // namespace estun