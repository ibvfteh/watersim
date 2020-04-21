#include "Renderer/Vulkan/Material/PipelineLayout.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Material/DescriptorSetLayout.h"

Estun::Renderer::Vulkan::PipelineLayout::PipelineLayout(const DescriptorSetLayout& descriptorSetLayout)
{
	VkDescriptorSetLayout descriptorSetLayouts[] = { descriptorSetLayout.GetLayout() };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	VK_CHECK_RESULT(vkCreatePipelineLayout(DeviceLocator::GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout), "create pipeline layout");
}

Estun::Renderer::Vulkan::PipelineLayout::~PipelineLayout()
{
	if (pipelineLayout != nullptr)
	{
		vkDestroyPipelineLayout(DeviceLocator::GetLogicalDevice(), pipelineLayout, nullptr);
		pipelineLayout = nullptr;
	}
}

VkPipelineLayout &Estun::Renderer::Vulkan::PipelineLayout::GetPipelineLayout()
{
    return pipelineLayout;
}
