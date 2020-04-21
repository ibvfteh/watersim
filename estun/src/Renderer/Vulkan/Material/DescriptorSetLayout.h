#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class DescriptorBinding;

class DescriptorSetLayout
{
public:
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout(DescriptorSetLayout &&) = delete;

    DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;
    DescriptorSetLayout &operator=(DescriptorSetLayout &&) = delete;

    DescriptorSetLayout(const std::vector<DescriptorBinding> &descriptorBindings);
    ~DescriptorSetLayout();

    VkDescriptorSetLayout GetLayout() const;

private:
    VkDescriptorSetLayout layout;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun