#pragma once

#include <memory>
#include <vector>
#include "Renderer/Vulkan/Material/DescriptorSets.h"

namespace Estun
{

namespace Renderer
{

namespace Vulkan
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

    void Bind();

    DescriptorSetLayout &GetDescriptorSetLayout() const;
    DescriptorSets &GetDescriptorSets();
    PipelineLayout &GetPipelineLayout() const;

private:
    std::unique_ptr<DescriptorPool> descriptorPool;
    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<DescriptorSets> descriptorSets;
    std::unique_ptr<PipelineLayout> pipelineLayout;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun