#include "renderer/vulkan/material/vulkan_descriptor_sets.h"
#include "renderer/vulkan/context/vulkan_device.h"
#include "renderer/vulkan/vulkan_context.h"

namespace estun
{
    VulkanDescriptorSets::VulkanDescriptorSets()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(*VulkanDeviceLocator::GetLogicalDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) 
        {
            ES_CORE_ASSERT("Failed to create descriptor set layout");
        }
    }

    VulkanDescriptorSets::~VulkanDescriptorSets()
    {
	    vkDestroyDescriptorSetLayout(*VulkanDeviceLocator::GetLogicalDevice(), descriptorSetLayout, nullptr);   
    }

    VkDescriptorSetLayout* VulkanDescriptorSets::GetDescriptorSetLayout()
    {
	    return &descriptorSetLayout;
    }

    const std::vector<VkDescriptorSet>* VulkanDescriptorSets::GetDescriptorSets() const
    {
    	return &descriptorSets;
    }		

	void VulkanDescriptorSets::CreateDescriptorSets(VkDescriptorPool* descriptorPool,
		                      const std::vector<VkBuffer>* uniformBuffers,
		                      VkImageView* textureImageView, VkSampler* textureSampler,
		                      const VkDeviceSize& bufferInfoSize)
    {
        uint32_t swapChainImagesSize = VulkanContextLocator::GetContext()->GetImageView()->GetSwapChainImageViewsVector()->size(); 
        std::vector<VkDescriptorSetLayout> layouts(swapChainImagesSize, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = *descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImagesSize);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapChainImagesSize);
        if (vkAllocateDescriptorSets(*VulkanDeviceLocator::GetLogicalDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) 
        {
            ES_CORE_ASSERT("Failed to allocate descriptor sets");
        }

        for (size_t i = 0; i < swapChainImagesSize; i++) 
        {
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = (*uniformBuffers)[i];
            bufferInfo.offset = 0;
            bufferInfo.range = bufferInfoSize;

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = *textureImageView;
            imageInfo.sampler = *textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(*VulkanDeviceLocator::GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
}
