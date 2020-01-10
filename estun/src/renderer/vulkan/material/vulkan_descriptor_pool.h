#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

namespace estun
{
	class VulkanDescriptorPool
	{
	private:
		VkDescriptorPool descriptorPool;

	public:
		VulkanDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes);
		~VulkanDescriptorPool();

    	static VkDescriptorPoolSize Descriptor(VkDescriptorType type, uint32_t count);
    	static VkDescriptorPoolSize UniformDescriptor();
    	static VkDescriptorPoolSize ImageDescriptor();
    	static VkDescriptorPoolSize StorageDescriptor();

		VkDescriptorPool* GetDescriptorPool();
	};
}
