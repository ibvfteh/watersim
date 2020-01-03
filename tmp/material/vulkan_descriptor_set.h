#pragma once

#include "renderer/vulkan/buffers/vulkan_uniform_buffer.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace estun
{
	class VulkanDescriptorSet
	{
	private:
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		std::vector<VkDescriptorSet> descriptorSets;

	public:
		VulkanDescriptorSet();
		~VulkanDescriptorSet();

		void CreateDescriptorSets(VkDescriptorPool* descriptorPool,
			                      const std::vector<VkBuffer>* uniformBuffers,
			                      VkImageView* textureImageView, VkSampler* textureSampler,
			                      const VkDeviceSize& bufferInfoSize = sizeof(UniformBufferObject));

		VkDescriptorSetLayout*              GetDescriptorSetLayout();
		VkPipelineLayout*                   GetPipelineLayout();
		const std::vector<VkDescriptorSet>* GetDescriptorSets() const;
	};
}
