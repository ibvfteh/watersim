#include "Renderer/Vulkan/Material/DescriptorSets.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Context/Utils.h"
#include "Renderer/Vulkan/Material/DescriptorPool.h"
#include "Renderer/Vulkan/Material/DescriptorSetLayout.h"
#include "Renderer/Vulkan/Material/Descriptable.h"

#include <array>
#include <utility>

Estun::Renderer::Vulkan::DescriptorSets::DescriptorSets(
	const DescriptorPool &descriptorPool,
	const DescriptorSetLayout &layout,
	std::map<uint32_t, VkDescriptorType> bindingTypes,
	const size_t size)
	: descriptorPool(descriptorPool),
	  bindingTypes(std::move(bindingTypes))
{
	std::vector<VkDescriptorSetLayout> layouts(size, layout.GetLayout());

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool.GetDescriptorPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(size);

	VK_CHECK_RESULT(vkAllocateDescriptorSets(DeviceLocator::GetLogicalDevice(), &allocInfo, descriptorSets.data()), "Failed to allocate descriptor sets");
}

Estun::Renderer::Vulkan::DescriptorSets::~DescriptorSets()
{
	//if (!descriptorSets.empty())
	//{
	//	vkFreeDescriptorSets(
	//		descriptorPool_.Device().Handle(),
	//		descriptorPool_.Handle(),
	//		static_cast<uint32_t>(descriptorSets_.size()),
	//		descriptorSets_.data());

	//	descriptorSets.clear();
	//}
}

VkWriteDescriptorSet Estun::Renderer::Vulkan::DescriptorSets::Bind(const uint32_t index, const uint32_t binding, const DescriptableInfo &info, const uint32_t count) const
{
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSets[index];
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = GetBindingType(binding);
	descriptorWrite.descriptorCount = count;
	if (info.Collide())
	{
		ES_CORE_ASSERT("Colliding descriptable info");
	}
	bool HasValue = false;
	if (info.bI.has_value())
	{
		descriptorWrite.pBufferInfo = &info.bI.value();
		bool HasValue = true;
	}
	if (info.iI.has_value())
	{
		descriptorWrite.pImageInfo = &info.iI.value();
		bool HasValue = true;
	}
	if (info.asI.has_value())
	{
		descriptorWrite.pNext = &info.asI.value();
		bool HasValue = true;
	}
	if (!HasValue)
	{
		ES_CORE_ASSERT("No descriptable info");
	}

	return descriptorWrite;
}

void Estun::Renderer::Vulkan::DescriptorSets::UpdateDescriptors(uint32_t index, const std::vector<VkWriteDescriptorSet> &descriptorWrites)
{
	vkUpdateDescriptorSets(
		DeviceLocator::GetLogicalDevice(),
		static_cast<uint32_t>(descriptorWrites.size()),
		descriptorWrites.data(), 0, nullptr);
}

VkDescriptorType Estun::Renderer::Vulkan::DescriptorSets::GetBindingType(uint32_t binding) const
{
	const auto it = bindingTypes.find(binding);
	if (it == bindingTypes.end())
	{
		ES_CORE_ASSERT("Binding not found");
	}

	return it->second;
}

VkDescriptorSet Estun::Renderer::Vulkan::DescriptorSets::GetDescriptorSet(uint32_t index) const
{
	return descriptorSets[index];
}
