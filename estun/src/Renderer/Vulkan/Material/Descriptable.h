#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

struct DescriptableInfo
{
    std::optional<VkDescriptorBufferInfo> bI;
    std::optional<VkDescriptorImageInfo> iI;
    std::optional<VkWriteDescriptorSetAccelerationStructureNV> asI;

    bool Collide() const
    {
        bool x1 = bI.has_value();
        bool x2 = iI.has_value();
        bool x3 = asI.has_value();
        return x1 && !x2 && !x3 || !x1 && x2 && !x3 || !x1 && !x2 && x3;
    }
};

class Descriptable
{
public:
    virtual DescriptableInfo GetInfo() { return DescriptableInfo(); };
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun