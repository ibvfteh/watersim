#include "renderer/material/descriptor.h"
#include "core/core.h"
#include "renderer/context.h"
#include "renderer/material/descriptor_binding.h"
#include "renderer/material/descriptor_sets.h"
#include "renderer/material/descriptor_pool.h"
#include "renderer/material/descriptor_set_layout.h"
#include "renderer/material/pipeline_layout.h"
//#include "renderer/ray_tracing/TopLevelAccelerationStructure.h"
#include "renderer/buffers/buffer.h"
#include "renderer/material/texture.h"

estun::Descriptor::Descriptor(const std::vector<DescriptorBinding> &descriptorBindings, const size_t maxSets)
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
        std::vector<DescriptableInfo> infos;
        uint32_t size = 0;
        for (int binding = 0; binding < descriptorBindings.size(); binding++)
        {
            size += descriptorBindings[binding].descriptable_.size();
        }
        infos.reserve(size);

        for (int binding = 0; binding < descriptorBindings.size(); binding++)
        {
            for (int j = 0; j < descriptorBindings[binding].descriptable_.size(); j++)
            {
                infos.push_back(descriptorBindings[binding].descriptable_[j]->GetInfo());
                descriptorWrites.push_back(descriptorSets->Bind(index, descriptorBindings[binding].binding_, infos.back()));
            }
        }
        descriptorSets->UpdateDescriptors(index, descriptorWrites);
    }

    pipelineLayout.reset(new PipelineLayout(*descriptorSetLayout));
}

estun::Descriptor::~Descriptor()
{
    pipelineLayout.reset();
    descriptorSets.reset();
    descriptorSetLayout.reset();
    descriptorPool.reset();
}

estun::DescriptorSetLayout &estun::Descriptor::GetDescriptorSetLayout() const
{
    return *descriptorSetLayout;
}

estun::DescriptorSets &estun::Descriptor::GetDescriptorSets()
{
    return *descriptorSets;
}

estun::PipelineLayout &estun::Descriptor::GetPipelineLayout() const
{
    return *pipelineLayout;
}

void estun::Descriptor::Bind(VkCommandBuffer &commandBuffer)
{
    VkDescriptorSet vkDescriptorSets[] = {descriptorSets->GetDescriptorSet(ContextLocator::GetImageIndex())};
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout->GetPipelineLayout(),
        0, 1,
        vkDescriptorSets,
        0, nullptr);
}

/*

void estun::Descriptor::Push(uint32_t index, Buffer &buffer)
{
    VkDescriptorBufferInfo uniformBufferInfo = {};
    uniformBufferInfo.buffer = buffer.GetBuffer();
    uniformBufferInfo.range = VK_WHOLE_SIZE;

    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorWrites.size(), uniformBufferInfo));
}

void estun::Descriptor::Push(uint32_t index, Texture &texture)
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture.GetImageView().GetImageView();
    imageInfo.sampler = texture.GetSampler().GetSampler();

    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorWrites.size(), imageInfo));
}

void estun::Descriptor::Push(uint32_t index, TopLevelAccelerationStructure &accelerationStructure)
{
    const auto accelerationStructureHandle = accelerationStructure.GetStructure();
    VkWriteDescriptorSetAccelerationStructureNV structureInfo = {};
    structureInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_NV;
    structureInfo.pNext = nullptr;
    structureInfo.accelerationStructureCount = 1;
    structureInfo.pAccelerationStructures = &accelerationStructureHandle;

    descriptorWrites.push_back(descriptorSets->Bind(index, descriptorWrites.size(), structureInfo));
}

void estun::Descriptor::Update(uint32_t index)
{
    descriptorSets->UpdateDescriptors(index, descriptorWrites);
}

void estun::Descriptor::BuildPipelineLayout()
{
    pipelineLayout.reset(new PipelineLayout(*descriptorSetLayout));
}

*/