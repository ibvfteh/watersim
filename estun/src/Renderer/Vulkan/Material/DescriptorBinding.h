#pragma once

#include <vulkan/vulkan.h>
#include <cstdint>
#include "Renderer/Vulkan/Material/Descriptable.h"
#include "Renderer/Vulkan/Buffers/UniformBuffer.h"
#include "Renderer/Vulkan/Buffers/StorageBuffer.h"
#include "Renderer/Vulkan/Material/Texture.h"
#include "Renderer/Vulkan/Context/ImageHolder.h"
//#include "Renderer/Vulkan/RayTracing/TopLevelAccelerationStructure.h"

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Descriptable;
class UniformBuffer;
template <class T>
class StorageBuffer;
class Texture;
class ImageHolder;
class TopLevelAccelerationStructure;

struct DescriptorBinding
{
    uint32_t binding_; // Slot to which the descriptor will be bound, corresponding to the layout index in the shader.
    std::vector<Descriptable> &descriptable_;
    uint32_t descriptorCount_; // Number of descriptors to bind
    VkDescriptorType type_;    // Type of the bound descriptor(s)
    VkShaderStageFlags stage_; // Shader stage at which the bound resources will be available

    DescriptorBinding(uint32_t binding, std::vector<Descriptable> &descriptable, uint32_t descriptorCount, VkDescriptorType type, VkShaderStageFlags stage)
        : binding_(binding),
          descriptable_(descriptable),
          descriptorCount_(descriptorCount),
          type_(type),
          stage_(stage)
    {
    }

    static DescriptorBinding Uniform(uint32_t binding, std::vector<class UniformBuffer> &uniformBuffers, VkShaderStageFlags stage)
    {
        std::vector<Descriptable> descriptables = {};
        for (int i = 0; i < uniformBuffers.size(); i++)
        {
            descriptables.push_back(uniformBuffers[i]);
        }
        return DescriptorBinding(binding, descriptables, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stage);
    }

    template <class T>
    static DescriptorBinding Storage(uint32_t binding, StorageBuffer<T> &storageBuffer, VkShaderStageFlags stage)
    {
        std::vector<Descriptable> descriptables = {storageBuffer};
        return DescriptorBinding(binding, descriptables, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, stage);
    }

    static DescriptorBinding Texture(uint32_t binding, Texture &texture, VkShaderStageFlags stage)
    {
        std::vector<Descriptable> descriptables = {texture};
        return DescriptorBinding(binding, descriptables, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
    }

    static DescriptorBinding Textures(uint32_t binding, std::vector<class Texture> &textures, VkShaderStageFlags stage)
    {
        std::vector<Descriptable> descriptables = {};
        for (int i = 0; i < textures.size(); i++)
        {
            descriptables.push_back(textures[i]);
        }
        return DescriptorBinding(binding, descriptables, static_cast<uint32_t>(textures.size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stage);
    }
/*
    static DescriptorBinding AccelerationStructure(uint32_t binding, TopLevelAccelerationStructure &tlas, VkShaderStageFlags stage)
    {
        std::vector<Descriptable> descriptables = {tlas};
        return DescriptorBinding(binding, descriptables, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, stage);
    }
*/
    static DescriptorBinding ImageHolder(uint32_t binding, ImageHolder &imageHolder, VkShaderStageFlags stage)
    {
        std::vector<Descriptable> descriptables = {imageHolder};
        return DescriptorBinding(binding, descriptables, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV, stage);
    }
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun