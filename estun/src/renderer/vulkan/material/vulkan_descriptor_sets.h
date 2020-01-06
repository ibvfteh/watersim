#pragma once

#include "renderer/vulkan/buffers/vulkan_uniform_buffer.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace estun
{
	class VulkanDescriptorSets
	{
	private:
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;

	public:
		VulkanDescriptorSets();
		~VulkanDescriptorSets();

		void CreateDescriptorSets(VkDescriptorPool* descriptorPool,
			                      const std::vector<VkBuffer>* uniformBuffers,
			                      VkImageView* textureImageView, VkSampler* textureSampler,
			                      const VkDeviceSize& bufferInfoSize = sizeof(UniformBufferObject));

		VkDescriptorSetLayout*              GetDescriptorSetLayout();
		const std::vector<VkDescriptorSet>* GetDescriptorSets() const;
	};
}
