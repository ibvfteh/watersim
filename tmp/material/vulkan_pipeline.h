#pragma once

#include "renderer/vulkan/swap_chain/vulkan_render_pass.h"

#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanGraphicsPipeline
    {
    private:
        VkPipelineLayout pipelineLayout;
        VkPipeline       graphicsPipeline;

    public:
        VulkanGraphicsPipeline(
                const std::string vertexShaderName,
                const std::string fragmentShaderName,
                VkDescriptorSetLayout* descriptorSetLayout
                );
        ~VulkanGraphicsPipeline();

        VkPipeline* GetGraphicsPipeline();
        VkPipelineLayout* GetPiplineLayout();
    }
}
