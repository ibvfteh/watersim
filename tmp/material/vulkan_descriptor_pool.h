#pragma once

#include <vulkan/vulkan.hpp>

namespace estun
{
	class VulkanDescriptorPool
	{
	private:
		VkDescriptorPool descriptorPool;

	public:
		VulkanDescriptorPool();
		~VulkanDescriptorPool();

		VkDescriptorPool* GetDescriptorPool();
	};
}
