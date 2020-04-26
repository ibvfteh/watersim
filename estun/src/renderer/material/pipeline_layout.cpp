#include "renderer/material/pipeline_layout.h"
#include "renderer/context/device.h"
#include "renderer/material/descriptor_set_layout.h"

estun::PipelineLayout::PipelineLayout(const DescriptorSetLayout& descriptorSetLayout)
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

estun::PipelineLayout::~PipelineLayout()
{
	if (pipelineLayout != nullptr)
	{
		vkDestroyPipelineLayout(DeviceLocator::GetLogicalDevice(), pipelineLayout, nullptr);
		pipelineLayout = nullptr;
	}
}

VkPipelineLayout &estun::PipelineLayout::GetPipelineLayout()
{
    return pipelineLayout;
}
