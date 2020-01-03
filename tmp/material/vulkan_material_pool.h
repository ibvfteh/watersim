#pragma once

#include <vulkan/vulkan.h>

#include "renderer/vulkan/material/vulkan_texture_image.h"
#include "renderer/vulkan/material/vulkan_texture_image_view.h"
#include "renderer/vulkan/material/vulkan_texture_sampler.h"
#include "renderer/vulkan/material/vulkan_pipeline.h"

#include <memory>
#include <unordered_map>
#include <string>

namespace estun
{
    class VulkanMaterialPool
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<VulkanTextureImage>>     textureImagePool;
        std::unordered_map<std::string, std::shared_ptr<VulkanTextureImageView>> textureImageViewPool;
        std::unordered_map<std::string, std::shared_ptr<VulkanTextureSampler>>   textureSamplerPool;

        std::unordered_map<std::string, std::shared_ptr<VulkanGraphicsPipeline>> pipelinePool;
    public:
        VulkanMaterialPool();
		~VulkanMaterialPool();

        std::shared_ptr<VulkanGraphicsPipeline> GetPipeline(
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath,
				VkDescriptorSetLayout* descriptorSetLayout);

		std::shared_ptr<VulkanTextureImage>     GetTextureImage(const std::string& texturePath);
		std::shared_ptr<VulkanTextureSampler>   GetTextureSampler(const std::string& texturePath);
		std::shared_ptr<VulkanTextureImageView> GetTextureImageView(const std::string& texturePath);

		void CleanUp();
		void RebuildPipelines();
    };

    class VulkanMaterialPoolLocator
    {
    private:
        static VulkanMaterialPool* pool;
    public:
        static void Provide(VulkanMaterialPool* pool) { pool = pool; };

        static std::shared_ptr<VulkanGraphicsPipeline> GetPipeline(
			const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath,
			VkDescriptorSetLayout* descriptorSetLayout)
        { return pool->GetPipeline(vertexShaderPath, fragmentShaderPath, descriptorSetLayout); };

		static std::shared_ptr<VulkanTextureImage>     GetTextureImage(const std::string& texturePath) { return pool->GetTextureImage(); };
		static std::shared_ptr<VulkanTextureSampler>   GetTextureSampler(const std::string& texturePath) { return pool->GetTextureSampler(); };
		static std::shared_ptr<VulkanTextureImageView> GetTextureImageView(const std::string& texturePath) { return pool->GetTextureImageView(); };
    };
}
