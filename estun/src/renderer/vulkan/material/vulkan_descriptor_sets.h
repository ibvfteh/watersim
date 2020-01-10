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
		VulkanDescriptorSets(std::vector<VkDescriptorSetLayoutBinding> bindings);
		~VulkanDescriptorSets();

    	static VkDescriptorSetLayoutBinding Binding(VkDescriptorType type, VkShaderStageFlags flags);
    	static VkDescriptorSetLayoutBinding UboBinding();
    	static VkDescriptorSetLayoutBinding SamplerLayoutBinding();

		void CreateDescriptorSets(VkDescriptorPool* descriptorPool,
			                      const std::vector<VkBuffer>* uniformBuffers,
			                      VkImageView* textureImageView, VkSampler* textureSampler,
			                      const VkDeviceSize& bufferInfoSize = sizeof(UniformBufferObject));

		void CreateComputeDescriptorSets(
			VkDescriptorPool* descriptorPool,
			VkBuffer* vertexBufferA,
			VkBuffer* vertexBufferB,
			VkBuffer* vertexBufferC,
			const VkDeviceSize& bufferInfoSize
		);

		void UpdateCompuiteDescriptorSets(
			VkBuffer* vertexBufferA,
			VkBuffer* vertexBufferB,
			VkBuffer* vertexBufferC,
			const VkDeviceSize& bufferInfoSize
			);

		VkDescriptorSetLayout*              GetDescriptorSetLayout();
		const std::vector<VkDescriptorSet>* GetDescriptorSets() const;
	};
}
