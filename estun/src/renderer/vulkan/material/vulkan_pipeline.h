#pragma once

#include "renderer/vulkan/swap_chain/vulkan_render_pass.h"
#include "renderer/vulkan/material/vulkan_descriptor_sets.h"

#include <vulkan/vulkan.h>
#include <string>

namespace estun
{
    class VulkanGraphicsPipeline
    {
    private:
        VkPipelineLayout pipelineLayout;
        VkPipeline       graphicsPipeline;
        VulkanDescriptorSets *descriptorSets;

    public:
        VulkanGraphicsPipeline(
                const std::string vertexShaderName,
                const std::string fragmentShaderName,
                VulkanDescriptorSets* shaderDescriptorSets
                );
        ~VulkanGraphicsPipeline();

        void Init(
                const std::string vertexShaderName,
                const std::string fragmentShaderName
                );
        void RebuildPipeline(
                const std::string vertexShaderName,
                const std::string fragmentShaderName
                );

        void Delete();

        VkPipeline* GetGraphicsPipeline();
        VkPipelineLayout* GetPipelineLayout();
        VulkanDescriptorSets* GetDescriptorSets();
    };

    class VulkanComputePipeline
    {
    private:
        VkPipelineLayout pipelineLayout;
        VkPipeline       computePipeline;
        VulkanDescriptorSets *descriptorSets;

    public:
        VulkanComputePipeline(
                const std::string computeShaderName,
                VulkanDescriptorSets* shaderDescriptorSets
                );
        ~VulkanComputePipeline();

        VkPipeline* GetComputePipeline();
        VkPipelineLayout* GetPipelineLayout();
        VulkanDescriptorSets* GetDescriptorSets();
    };
}
