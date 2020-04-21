#pragma once

#include <vulkan/vulkan.h>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class DescriptorSetLayout;

class PipelineLayout 
{
public:
    PipelineLayout(const PipelineLayout &) = delete;
    PipelineLayout(PipelineLayout &&) = delete;

    PipelineLayout &operator=(const PipelineLayout &) = delete;
    PipelineLayout &operator=(PipelineLayout &&) = delete;

    PipelineLayout(const DescriptorSetLayout &descriptorSetLayout);
    ~PipelineLayout();

    VkPipelineLayout &GetPipelineLayout();

private:
    VkPipelineLayout pipelineLayout;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun