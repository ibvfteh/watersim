#include "Renderer/Vulkan/Material/Descriptor.h"
#include "Core/Core.h"
#include "Renderer/Vulkan/Context.h"
#include "Renderer/Vulkan/Material/DescriptorBinding.h"
#include "Renderer/Vulkan/Material/DescriptorSets.h"
#include "Renderer/Vulkan/Material/DescriptorPool.h"
#include "Renderer/Vulkan/Material/DescriptorSetLayout.h"
#include "Renderer/Vulkan/Material/PipelineLayout.h"
//#include "Renderer/Vulkan/RayTracing/TopLevelAccelerationStructure.h"
#include "Renderer/Vulkan/Buffers/Buffer.h"
#include "Renderer/Vulkan/Material/Texture.h"

Estun::Renderer::Vulkan::Descriptor::Descriptor(const std::vector<DescriptorBinding> &descriptorBindings, const size_t maxSets)
{
    std::map<uint32_t, VkDescriptorType> bindingTypes;

    for (const auto &binding : descriptorBindings)
    {
        if (!bindingTypes.insert(std::make_pair(binding.binding_, binding.type_)).second)
        {
            ES_CORE_ASSERT("Binding collision");
        }
    }

    descriptorPool.reset(new DescriptorPool(descriptorBindings, maxSets));
    descriptorSetLayout.reset(new DescriptorSetLayout(descriptorBindings));
    descriptorSets.reset(new DescriptorSets(*descriptorPool, *descriptorSetLayout, bindingTypes, maxSets));

    for (int index = 0; index < maxSets; index++)
    {
        std::vector<VkWriteDescriptorSet> descriptorWrites;

        for (int binding = 0; binding < descriptorBindings.size(); binding++)
        {
            for (int j = 0; j < descriptorBindings[binding].descriptable_.size(); j++)
            {
                auto info = descriptorBindings[binding].descriptable_[j].GetInfo();
                if (info.bI.has_value())
                {
                    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorBindings[index].binding_, info));
                }
            }
        }
        descriptorSets->UpdateDescriptors(index, descriptorWrites);
    }

    pipelineLayout.reset(new PipelineLayout(*descriptorSetLayout));
}

Estun::Renderer::Vulkan::Descriptor::~Descriptor()
{
    pipelineLayout.reset();
    descriptorSets.reset();
    descriptorSetLayout.reset();
    descriptorPool.reset();
}

Estun::Renderer::Vulkan::DescriptorSetLayout &Estun::Renderer::Vulkan::Descriptor::GetDescriptorSetLayout() const
{
    return *descriptorSetLayout;
}

Estun::Renderer::Vulkan::DescriptorSets &Estun::Renderer::Vulkan::Descriptor::GetDescriptorSets()
{
    return *descriptorSets;
}

Estun::Renderer::Vulkan::PipelineLayout &Estun::Renderer::Vulkan::Descriptor::GetPipelineLayout() const
{
    return *pipelineLayout;
}

void Estun::Renderer::Vulkan::Descriptor::Bind()
{
    VkDescriptorSet vkDescriptorSets[] = {descriptorSets->GetDescriptorSet(ContextLocator::GetImageIndex())};
    vkCmdBindDescriptorSets(
        ContextLocator::GetCurrentCommandBuffer(),
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout->GetPipelineLayout(),
        0, 1,
        vkDescriptorSets,
        0, nullptr);
}

/*

void Estun::Renderer::Vulkan::Descriptor::Push(uint32_t index, Buffer &buffer)
{
    VkDescriptorBufferInfo uniformBufferInfo = {};
    uniformBufferInfo.buffer = buffer.GetBuffer();
    uniformBufferInfo.range = VK_WHOLE_SIZE;

    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorWrites.size(), uniformBufferInfo));
}

void Estun::Renderer::Vulkan::Descriptor::Push(uint32_t index, Texture &texture)
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture.GetImageView().GetImageView();
    imageInfo.sampler = texture.GetSampler().GetSampler();

    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorWrites.size(), imageInfo));
}

void Estun::Renderer::Vulkan::Descriptor::Push(uint32_t index, TopLevelAccelerationStructure &accelerationStructure)
{
    const auto accelerationStructureHandle = accelerationStructure.GetStructure();
    VkWriteDescriptorSetAccelerationStructureNV structureInfo = {};
    structureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
    structureInfo.pNext = nullptr;
    structureInfo.accelerationStructureCount = 1;
    structureInfo.pAccelerationStructures = &accelerationStructureHandle;

    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorWrites.size(), structureInfo));
}

void Estun::Renderer::Vulkan::Descriptor::Update(uint32_t index)
{
    descriptorSets->UpdateDescriptors(index, descriptorWrites);
}

void Estun::Renderer::Vulkan::Descriptor::BuildPipelineLayout()
{
    pipelineLayout.reset(new PipelineLayout(*descriptorSetLayout));
}

*/